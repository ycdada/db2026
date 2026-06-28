/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "log_recovery.h"

#include <algorithm>
#include <fstream>

#include "record/bitmap.h"

namespace {

std::vector<char> make_index_key(const RmRecord &record, const IndexMeta &index) {
    std::vector<char> key(index.col_tot_len);
    int offset = 0;
    for (int i = 0; i < index.col_num; ++i) {
        memcpy(key.data() + offset, record.data + index.cols[i].offset, index.cols[i].len);
        offset += index.cols[i].len;
    }
    return key;
}

}  // namespace

int RecoveryManager::checkpoint_offset() {
    std::ifstream ifs(sm_manager_->db_.name() + "/" + RESTART_FILE_NAME);
    int offset = 0;
    if (ifs.is_open()) {
        ifs >> offset;
    }
    return std::max(0, offset);
}

void RecoveryManager::load_logs() {
    logs_.clear();
    int offset = checkpoint_offset();
    char header[LOG_HEADER_SIZE];
    while (true) {
        int n = disk_manager_->read_log(header, LOG_HEADER_SIZE, offset);
        if (n == 0 || n == -1) {
            break;
        }
        if (n < LOG_HEADER_SIZE) {
            break;
        }
        uint32_t total_len = *reinterpret_cast<uint32_t *>(header + OFFSET_LOG_TOT_LEN);
        if (total_len < LOG_HEADER_SIZE || total_len > LOG_BUFFER_SIZE) {
            break;
        }
        std::vector<char> data(total_len);
        int full = disk_manager_->read_log(data.data(), total_len, offset);
        if (full != static_cast<int>(total_len)) {
            break;
        }
        auto record = LogManager::deserialize_log_record(data.data(), data.size());
        if (record == nullptr) {
            break;
        }
        logs_.push_back(std::move(record));
        offset += total_len;
    }
}

/**
 * @description: analyze阶段，需要获得脏页表（DPT）和未完成的事务列表（ATT）
 */
void RecoveryManager::analyze() {
    load_logs();
    committed_txns_.clear();
    active_txns_.clear();
    undo_logs_.clear();

    for (auto &record : logs_) {
        switch (record->log_type_) {
            case LogType::begin:
                active_txns_.insert(record->log_tid_);
                break;
            case LogType::commit:
                active_txns_.erase(record->log_tid_);
                committed_txns_.insert(record->log_tid_);
                break;
            case LogType::ABORT:
                active_txns_.erase(record->log_tid_);
                break;
            case LogType::INSERT:
            case LogType::DELETE:
            case LogType::UPDATE:
                if (!committed_txns_.count(record->log_tid_)) {
                    active_txns_.insert(record->log_tid_);
                }
                break;
            default:
                break;
        }
    }
    for (auto it = logs_.rbegin(); it != logs_.rend(); ++it) {
        LogRecord *record = it->get();
        if ((record->log_type_ == LogType::INSERT || record->log_type_ == LogType::DELETE ||
             record->log_type_ == LogType::UPDATE) &&
            active_txns_.count(record->log_tid_)) {
            undo_logs_.push_back(record);
        }
    }
}

bool RecoveryManager::record_exists(RmFileHandle *fh, const Rid &rid) {
    try {
        return fh->is_record(rid);
    } catch (...) {
        return false;
    }
}

void RecoveryManager::ensure_record_slot(RmFileHandle *fh, const Rid &rid) {
    auto hdr = fh->get_file_hdr();
    while (rid.page_no >= hdr.num_pages) {
        auto page = fh->create_new_page_handle();
        buffer_pool_manager_->unpin_page(page.page->get_page_id(), true);
        hdr = fh->get_file_hdr();
    }
}

void RecoveryManager::insert_indexes(const std::string &tab_name, const RmRecord &record, const Rid &rid) {
    TabMeta &tab = sm_manager_->db_.get_table(tab_name);
    for (auto &index : tab.indexes) {
        auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name, index.cols)).get();
        auto key = make_index_key(record, index);
        std::vector<Rid> result;
        if (!ih->get_value(key.data(), &result, nullptr)) {
            ih->insert_entry(key.data(), rid, nullptr);
        }
    }
}

void RecoveryManager::delete_indexes(const std::string &tab_name, const RmRecord &record) {
    TabMeta &tab = sm_manager_->db_.get_table(tab_name);
    for (auto &index : tab.indexes) {
        auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name, index.cols)).get();
        auto key = make_index_key(record, index);
        try {
            ih->delete_entry(key.data(), nullptr);
        } catch (...) {
        }
    }
}

void RecoveryManager::redo_insert(const std::string &tab_name, const Rid &rid, const RmRecord &record) {
    auto fh = sm_manager_->fhs_.at(tab_name).get();
    ensure_record_slot(fh, rid);
    if (record_exists(fh, rid)) {
        auto old = fh->get_record(rid, nullptr);
        delete_indexes(tab_name, *old);
        fh->update_record(rid, record.data, nullptr);
    } else {
        fh->insert_record(rid, record.data);
    }
    insert_indexes(tab_name, record, rid);
}

void RecoveryManager::redo_delete(const std::string &tab_name, const Rid &rid, const RmRecord &record) {
    auto fh = sm_manager_->fhs_.at(tab_name).get();
    if (!record_exists(fh, rid)) {
        return;
    }
    auto old = fh->get_record(rid, nullptr);
    delete_indexes(tab_name, *old);
    fh->delete_record(rid, nullptr);
    (void)record;
}

void RecoveryManager::redo_update(const std::string &tab_name, const Rid &rid, const RmRecord &old_record,
                                  const RmRecord &new_record) {
    auto fh = sm_manager_->fhs_.at(tab_name).get();
    ensure_record_slot(fh, rid);
    if (record_exists(fh, rid)) {
        auto old = fh->get_record(rid, nullptr);
        delete_indexes(tab_name, *old);
        fh->update_record(rid, new_record.data, nullptr);
    } else {
        fh->insert_record(rid, new_record.data);
    }
    insert_indexes(tab_name, new_record, rid);
    (void)old_record;
}

void RecoveryManager::apply_redo(LogRecord *record) {
    switch (record->log_type_) {
        case LogType::INSERT: {
            auto *log = static_cast<InsertLogRecord *>(record);
            redo_insert(log->table_name_, log->rid_, log->insert_value_);
            break;
        }
        case LogType::DELETE: {
            auto *log = static_cast<DeleteLogRecord *>(record);
            redo_delete(log->table_name_, log->rid_, log->delete_value_);
            break;
        }
        case LogType::UPDATE: {
            auto *log = static_cast<UpdateLogRecord *>(record);
            redo_update(log->table_name_, log->rid_, log->old_value_, log->new_value_);
            break;
        }
        default:
            break;
    }
}

/**
 * @description: 重做所有未落盘的操作
 */
void RecoveryManager::redo() {
    for (auto &record : logs_) {
        if (committed_txns_.count(record->log_tid_)) {
            apply_redo(record.get());
        }
    }
}

void RecoveryManager::apply_undo(LogRecord *record) {
    switch (record->log_type_) {
        case LogType::INSERT: {
            auto *log = static_cast<InsertLogRecord *>(record);
            redo_delete(log->table_name_, log->rid_, log->insert_value_);
            break;
        }
        case LogType::DELETE: {
            auto *log = static_cast<DeleteLogRecord *>(record);
            redo_insert(log->table_name_, log->rid_, log->delete_value_);
            break;
        }
        case LogType::UPDATE: {
            auto *log = static_cast<UpdateLogRecord *>(record);
            redo_update(log->table_name_, log->rid_, log->new_value_, log->old_value_);
            break;
        }
        default:
            break;
    }
}

/**
 * @description: 回滚未完成的事务
 */
void RecoveryManager::undo() {
    for (auto *record : undo_logs_) {
        apply_undo(record);
    }
}
