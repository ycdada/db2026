/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "transaction_manager.h"
#include "record/rm_file_handle.h"
#include "system/sm_manager.h"

#include <algorithm>

std::unordered_map<txn_id_t, Transaction *> TransactionManager::txn_map = {};

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

void insert_index_entries(SmManager *sm_manager, const std::string &tab_name, const TabMeta &tab,
                          const RmRecord &record, const Rid &rid, Transaction *txn) {
    for (auto &index : tab.indexes) {
        auto ih = sm_manager->ihs_.at(sm_manager->get_ix_manager()->get_index_name(tab_name, index.cols)).get();
        auto key = make_index_key(record, index);
        ih->insert_entry(key.data(), rid, txn);
    }
}

void delete_index_entries(SmManager *sm_manager, const std::string &tab_name, const TabMeta &tab,
                          const RmRecord &record, Transaction *txn) {
    for (auto &index : tab.indexes) {
        auto ih = sm_manager->ihs_.at(sm_manager->get_ix_manager()->get_index_name(tab_name, index.cols)).get();
        auto key = make_index_key(record, index);
        ih->delete_entry(key.data(), txn);
    }
}

bool eval_txn_conds(const char *rec_data, const std::vector<ColMeta> &cols,
                    const std::vector<Condition> &conds) {
    for (auto &cond : conds) {
        const ColMeta *lhs_meta = nullptr;
        for (auto &col : cols) {
            if (col.tab_name == cond.lhs_col.tab_name && col.name == cond.lhs_col.col_name) {
                lhs_meta = &col;
                break;
            }
        }
        if (lhs_meta == nullptr) return false;
        const char *lhs_data = rec_data + lhs_meta->offset;
        auto compare_int = [&](int lhs, int rhs) {
            switch (cond.op) {
                case OP_EQ: return lhs == rhs;
                case OP_NE: return lhs != rhs;
                case OP_LT: return lhs < rhs;
                case OP_GT: return lhs > rhs;
                case OP_LE: return lhs <= rhs;
                case OP_GE: return lhs >= rhs;
            }
            return false;
        };
        auto compare_float = [&](float lhs, float rhs) {
            switch (cond.op) {
                case OP_EQ: return lhs == rhs;
                case OP_NE: return lhs != rhs;
                case OP_LT: return lhs < rhs;
                case OP_GT: return lhs > rhs;
                case OP_LE: return lhs <= rhs;
                case OP_GE: return lhs >= rhs;
            }
            return false;
        };
        auto compare_string = [&](const std::string &lhs, const std::string &rhs) {
            switch (cond.op) {
                case OP_EQ: return lhs == rhs;
                case OP_NE: return lhs != rhs;
                case OP_LT: return lhs < rhs;
                case OP_GT: return lhs > rhs;
                case OP_LE: return lhs <= rhs;
                case OP_GE: return lhs >= rhs;
            }
            return false;
        };

        if (cond.is_rhs_val) {
            if (lhs_meta->type == TYPE_INT) {
                if (!compare_int(*(const int *)lhs_data, cond.rhs_val.int_val)) return false;
            } else if (lhs_meta->type == TYPE_FLOAT) {
                if (!compare_float(*(const float *)lhs_data, cond.rhs_val.float_val)) return false;
            } else {
                std::string lhs(lhs_data, lhs_meta->len);
                lhs.resize(strlen(lhs.c_str()));
                if (!compare_string(lhs, cond.rhs_val.str_val)) return false;
            }
        } else {
            const ColMeta *rhs_meta = nullptr;
            for (auto &col : cols) {
                if (col.tab_name == cond.rhs_col.tab_name && col.name == cond.rhs_col.col_name) {
                    rhs_meta = &col;
                    break;
                }
            }
            if (rhs_meta == nullptr) return false;
            const char *rhs_data = rec_data + rhs_meta->offset;
            if (lhs_meta->type == TYPE_INT) {
                if (!compare_int(*(const int *)lhs_data, *(const int *)rhs_data)) return false;
            } else if (lhs_meta->type == TYPE_FLOAT) {
                if (!compare_float(*(const float *)lhs_data, *(const float *)rhs_data)) return false;
            } else {
                std::string lhs(lhs_data, lhs_meta->len);
                lhs.resize(strlen(lhs.c_str()));
                std::string rhs(rhs_data, rhs_meta->len);
                rhs.resize(strlen(rhs.c_str()));
                if (!compare_string(lhs, rhs)) return false;
            }
        }
    }
    return true;
}

}

/**
 * @description: 事务的开始方法
 * @return {Transaction*} 开始事务的指针
 * @param {Transaction*} txn 事务指针，空指针代表需要创建新事务，否则开始已有事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
Transaction * TransactionManager::begin(Transaction* txn, LogManager* log_manager, IsolationLevel isolation_level) {
    // Todo:
    // 1. 判断传入事务参数是否为空指针
    // 2. 如果为空指针，创建新事务
    // 3. 把开始事务加入到全局事务表中
    // 4. 返回当前事务指针
    // 如果需要支持MVCC请在上述过程中添加代码
    bool new_txn = txn == nullptr;
    if (new_txn) {
        // 创建新事务
        txn_id_t new_txn_id = next_txn_id_++;
        txn = new Transaction(new_txn_id, isolation_level);
        txn->set_mvcc_enabled(isolation_level == IsolationLevel::SNAPSHOT_ISOLATION ||
                              isolation_level == IsolationLevel::SERIALIZABLE);
        txn->set_state(TransactionState::DEFAULT);

        std::unique_lock<std::mutex> lock(latch_);
        txn_map[new_txn_id] = txn;
    }
    // 如果是已有事务，直接复用
    txn->set_state(TransactionState::GROWING);
    if (new_txn && txn->is_mvcc()) {
        timestamp_t start_ts = last_commit_ts_.load();
        txn->set_start_ts(start_ts);
        txn->set_read_ts(start_ts);
        running_txns_.AddTxn(start_ts);
    }
    return txn;
}

bool TransactionManager::IsMvccTxn(Transaction *txn) const {
    return txn != nullptr && txn->is_mvcc();
}

std::string TransactionManager::MvccKey(const std::string &tab_name, const Rid &rid) const {
    return tab_name + "#" + std::to_string(rid.page_no) + "#" + std::to_string(rid.slot_no);
}

TransactionManager::MvccEntry &TransactionManager::EnsureMvccEntryLocked(
    const std::string &tab_name, const Rid &rid, const RmRecord *physical) {
    std::string key = MvccKey(tab_name, rid);
    auto [it, inserted] = mvcc_versions_.try_emplace(key);
    auto &entry = it->second;
    if (inserted && physical != nullptr) {
        entry.exists = true;
        entry.is_deleted = false;
        entry.commit_ts = 0;
        entry.writer_txn = INVALID_TXN_ID;
        entry.last_writer_txn = INVALID_TXN_ID;
        entry.tab_name = tab_name;
        entry.rid = rid;
        entry.has_head_record = true;
        entry.head_record = *physical;
    } else if (inserted) {
        entry.tab_name = tab_name;
        entry.rid = rid;
    }
    return entry;
}

std::optional<RmRecord> TransactionManager::VisibleRecordLocked(
    const std::string &key, const MvccEntry &entry, const RmRecord &physical, Transaction *txn) const {
    if (!IsMvccTxn(txn)) {
        if (entry.writer_txn != INVALID_TXN_ID) {
            for (auto it = entry.undo_versions.rbegin(); it != entry.undo_versions.rend(); ++it) {
                if (it->writer_txn == INVALID_TXN_ID) {
                    if (it->is_deleted) return std::nullopt;
                    return RmRecord(it->record);
                }
            }
            return std::nullopt;
        }
        if (!entry.exists || entry.is_deleted) return std::nullopt;
        return RmRecord(physical);
    }

    txn_id_t txn_id = txn->get_transaction_id();
    if (entry.writer_txn == txn_id) {
        if (!entry.exists || entry.is_deleted) return std::nullopt;
        return RmRecord(physical);
    }

    timestamp_t read_ts = txn->get_start_ts();
    if (entry.writer_txn == INVALID_TXN_ID && entry.exists && entry.commit_ts <= read_ts) {
        if (entry.is_deleted) return std::nullopt;
        return RmRecord(physical);
    }

    for (auto it = entry.undo_versions.rbegin(); it != entry.undo_versions.rend(); ++it) {
        bool committed = it->writer_txn == INVALID_TXN_ID;
        if (committed && it->commit_ts <= read_ts) {
            if (it->is_deleted) return std::nullopt;
            return RmRecord(it->record);
        }
    }
    return std::nullopt;
}

std::unique_ptr<RmRecord> TransactionManager::GetVisibleRecord(const std::string &tab_name, const Rid &rid,
                                                               const RmRecord &physical, Transaction *txn) {
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    std::string key = MvccKey(tab_name, rid);
    auto it = mvcc_versions_.find(key);
    if (it == mvcc_versions_.end()) {
        return std::make_unique<RmRecord>(physical);
    }
    auto visible = VisibleRecordLocked(key, it->second, physical, txn);
    if (!visible.has_value()) {
        return nullptr;
    }
    return std::make_unique<RmRecord>(*visible);
}

bool TransactionManager::IsVisible(const std::string &tab_name, const Rid &rid,
                                   const RmRecord &physical, Transaction *txn) {
    return GetVisibleRecord(tab_name, rid, physical, txn) != nullptr;
}

timestamp_t TransactionManager::GetWatermark() {
    return running_txns_.GetWatermark();
}

void TransactionManager::GarbageCollection() {
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    CleanupSerializableStateLocked();
}

void TransactionManager::CheckMvccWriteConflict(const std::string &tab_name, const Rid &rid,
                                                const RmRecord &old_rec, Transaction *txn) {
    if (!IsMvccTxn(txn)) return;
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    std::string key = MvccKey(tab_name, rid);
    auto it = mvcc_versions_.find(key);
    if (it == mvcc_versions_.end()) {
        return;
    }
    auto &entry = it->second;
    txn_id_t txn_id = txn->get_transaction_id();
    if (entry.writer_txn != INVALID_TXN_ID && entry.writer_txn != txn_id) {
        throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
    }
    if (entry.writer_txn != txn_id && entry.commit_ts > txn->get_start_ts()) {
        throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
    }
}

void TransactionManager::PrepareWriteLocked(const std::string &tab_name, const Rid &rid, const RmRecord &old_rec,
                                            Transaction *txn, bool inserted_record) {
    std::string key = MvccKey(tab_name, rid);
    bool had_entry = mvcc_versions_.find(key) != mvcc_versions_.end();
    auto &entry = EnsureMvccEntryLocked(tab_name, rid, inserted_record ? nullptr : &old_rec);
    txn_id_t txn_id = txn->get_transaction_id();

    if (entry.writer_txn != INVALID_TXN_ID && entry.writer_txn != txn_id) {
        throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
    }
    if (entry.writer_txn != txn_id && entry.commit_ts > txn->get_start_ts()) {
        throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
    }

    if (txn->mvcc_write_keys().count(key) == 0) {
        entry.had_entry_on_abort = had_entry;
        if (had_entry) {
            MvccEntry snapshot = entry;
            snapshot.rollback_delete_physical = false;
            snapshot.rollback_restore_physical = false;
            snapshot.restore_snapshot = nullptr;
            snapshot.had_entry_on_abort = false;
            entry.restore_snapshot = std::make_shared<MvccEntry>(snapshot);
        } else {
            entry.restore_snapshot = nullptr;
        }
        entry.rollback_delete_physical = inserted_record;
        entry.rollback_restore_physical = !inserted_record;
        if (!inserted_record) {
            entry.rollback_record = old_rec;
            entry.undo_versions.emplace_back(old_rec, entry.is_deleted, entry.commit_ts, entry.writer_txn);
        }
        txn->add_mvcc_write_key(key);
    }
}

bool TransactionManager::HasDangerousStructureLocked(txn_id_t pivot) const {
    auto in = rw_in_.find(pivot);
    auto out = rw_out_.find(pivot);
    if (in == rw_in_.end() || out == rw_out_.end()) {
        return false;
    }
    return !in->second.empty() && !out->second.empty();
}

void TransactionManager::RememberSerializableTxnLocked(Transaction *txn) {
    if (txn == nullptr || txn->get_isolation_level() != IsolationLevel::SERIALIZABLE) {
        return;
    }
    ser_txn_start_ts_.try_emplace(txn->get_transaction_id(), txn->get_start_ts());
}

void TransactionManager::AddRwEdgeLocked(txn_id_t reader, txn_id_t writer, Transaction *current_txn) {
    if (reader == INVALID_TXN_ID || writer == INVALID_TXN_ID || reader == writer) {
        return;
    }
    auto inserted = rw_edges_.insert({reader, writer}).second;
    if (!inserted) {
        return;
    }
    rw_out_[reader].insert(writer);
    rw_in_[writer].insert(reader);
    if (HasDangerousStructureLocked(reader) || HasDangerousStructureLocked(writer)) {
        throw TransactionAbortException(current_txn->get_transaction_id(), AbortReason::SSI_CONFLICT);
    }
}

void TransactionManager::CheckSerializableWriteLocked(const std::string &tab_name, const Rid &rid,
                                                      const RmRecord *old_rec, const RmRecord *new_rec,
                                                      Transaction *txn) {
    if (!IsMvccTxn(txn) || txn->get_isolation_level() != IsolationLevel::SERIALIZABLE) {
        return;
    }
    std::string key = MvccKey(tab_name, rid);
    txn_id_t writer = txn->get_transaction_id();
    RememberSerializableTxnLocked(txn);
	    for (auto &entry : serializable_reads_) {
	        txn_id_t reader = entry.first;
	        if (reader == writer) continue;
	        auto finish_it = ser_txn_finish_ts_.find(reader);
	        if (finish_it != ser_txn_finish_ts_.end() && finish_it->second <= txn->get_start_ts()) {
	            continue;
	        }
	        for (auto &read : entry.second) {
            if (read.tab_name != tab_name) continue;
            bool match_record = read.record_keys.count(key) != 0;
            bool match_old = old_rec != nullptr && eval_txn_conds(old_rec->data, read.cols, read.conds);
            bool match_new = new_rec != nullptr && eval_txn_conds(new_rec->data, read.cols, read.conds);
            if (match_record || match_old || match_new) {
                AddRwEdgeLocked(reader, writer, txn);
            }
        }
    }
}

void TransactionManager::MvccInsertLocked(const std::string &tab_name, const Rid &rid,
                                          const RmRecord &new_rec, Transaction *txn) {
    RmRecord dummy(new_rec);
    CheckSerializableWriteLocked(tab_name, rid, nullptr, &new_rec, txn);
    PrepareWriteLocked(tab_name, rid, dummy, txn, true);
    auto &entry = mvcc_versions_[MvccKey(tab_name, rid)];
    entry.exists = true;
    entry.is_deleted = false;
    entry.commit_ts = INVALID_TS;
    entry.writer_txn = txn->get_transaction_id();
    entry.last_writer_txn = txn->get_transaction_id();
    entry.has_head_record = true;
    entry.head_record = new_rec;
}

void TransactionManager::MvccInsert(const std::string &tab_name, const Rid &rid,
                                    const RmRecord &new_rec, Transaction *txn) {
    if (!IsMvccTxn(txn)) return;
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    MvccInsertLocked(tab_name, rid, new_rec, txn);
}

Rid TransactionManager::MvccInsertWithPhysical(const std::string &tab_name, const RmRecord &new_rec, Transaction *txn,
                                               const std::function<Rid()> &insert_fn,
                                               const std::function<void(const Rid &)> &rollback_fn) {
    if (!IsMvccTxn(txn)) {
        return insert_fn();
    }
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    Rid rid = insert_fn();
    try {
        MvccInsertLocked(tab_name, rid, new_rec, txn);
    } catch (...) {
        rollback_fn(rid);
        throw;
    }
    return rid;
}

void TransactionManager::MvccUpdate(const std::string &tab_name, const Rid &rid, const RmRecord &old_rec,
                                    const RmRecord &new_rec, Transaction *txn) {
    if (!IsMvccTxn(txn)) return;
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    CheckSerializableWriteLocked(tab_name, rid, &old_rec, &new_rec, txn);
    PrepareWriteLocked(tab_name, rid, old_rec, txn, false);
    auto &entry = mvcc_versions_[MvccKey(tab_name, rid)];
    entry.exists = true;
    entry.is_deleted = false;
    entry.commit_ts = INVALID_TS;
    entry.writer_txn = txn->get_transaction_id();
    entry.last_writer_txn = txn->get_transaction_id();
    entry.has_head_record = true;
    entry.head_record = new_rec;
}

void TransactionManager::MvccDelete(const std::string &tab_name, const Rid &rid,
                                    const RmRecord &old_rec, Transaction *txn) {
    if (!IsMvccTxn(txn)) return;
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    CheckSerializableWriteLocked(tab_name, rid, &old_rec, nullptr, txn);
    PrepareWriteLocked(tab_name, rid, old_rec, txn, false);
    auto &entry = mvcc_versions_[MvccKey(tab_name, rid)];
    entry.exists = true;
    entry.is_deleted = true;
    entry.commit_ts = INVALID_TS;
    entry.writer_txn = txn->get_transaction_id();
    entry.last_writer_txn = txn->get_transaction_id();
    entry.has_head_record = true;
    entry.head_record = old_rec;
}

void TransactionManager::RegisterSerializableRead(const std::string &tab_name, const std::vector<ColMeta> &cols,
                                                  const std::vector<Condition> &conds,
                                                  const std::vector<Rid> &returned_rids, Transaction *txn) {
    if (!IsMvccTxn(txn) || txn->get_isolation_level() != IsolationLevel::SERIALIZABLE) {
        return;
    }
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    RememberSerializableTxnLocked(txn);
    PredicateRead read;
    read.txn_id = txn->get_transaction_id();
    read.tab_name = tab_name;
    read.cols = cols;
    read.conds = conds;
    for (auto &rid : returned_rids) {
        read.record_keys.insert(MvccKey(tab_name, rid));
    }
	    for (auto &entry_pair : mvcc_versions_) {
	        auto &entry = entry_pair.second;
	        if (entry.tab_name != tab_name || !entry.has_head_record) {
	            continue;
	        }
        txn_id_t writer = entry.writer_txn != INVALID_TXN_ID ? entry.writer_txn : entry.last_writer_txn;
        if (writer == INVALID_TXN_ID || writer == read.txn_id) {
            continue;
        }
        bool invisible_active = entry.writer_txn != INVALID_TXN_ID && entry.writer_txn != read.txn_id;
        bool invisible_committed = entry.writer_txn == INVALID_TXN_ID && entry.commit_ts > txn->get_start_ts();
        if (!invisible_active && !invisible_committed) {
            continue;
        }
        bool key_returned = read.record_keys.count(entry_pair.first) != 0;
	        bool current_matches = entry.exists && !entry.is_deleted &&
	                               eval_txn_conds(entry.head_record.data, cols, conds);
        bool old_matches = false;
        for (auto it = entry.undo_versions.rbegin(); it != entry.undo_versions.rend(); ++it) {
            bool committed = it->writer_txn == INVALID_TXN_ID;
            if (committed && it->commit_ts <= txn->get_start_ts()) {
                old_matches = !it->is_deleted && eval_txn_conds(it->record.data, cols, conds);
                break;
            }
        }
        if (key_returned || current_matches || old_matches) {
            AddRwEdgeLocked(read.txn_id, writer, txn);
        }
    }
    serializable_reads_[read.txn_id].push_back(std::move(read));
}

void TransactionManager::CleanupSerializableStateLocked() {
    timestamp_t watermark = running_txns_.GetWatermark();
    std::vector<txn_id_t> removable;
    for (auto &entry : ser_txn_finish_ts_) {
        if (entry.second <= watermark) {
            removable.push_back(entry.first);
        }
    }
    for (txn_id_t txn_id : removable) {
        serializable_reads_.erase(txn_id);
        rw_in_.erase(txn_id);
        rw_out_.erase(txn_id);
        for (auto &entry : rw_in_) {
            entry.second.erase(txn_id);
        }
        for (auto &entry : rw_out_) {
            entry.second.erase(txn_id);
        }
        for (auto it = rw_edges_.begin(); it != rw_edges_.end(); ) {
            if (it->first == txn_id || it->second == txn_id) {
                it = rw_edges_.erase(it);
            } else {
                ++it;
            }
        }
        ser_txn_start_ts_.erase(txn_id);
        ser_txn_finish_ts_.erase(txn_id);
    }
}

void TransactionManager::ClearTxnStateLocked(Transaction *txn) {
    if (txn == nullptr) return;
    txn_id_t txn_id = txn->get_transaction_id();
    serializable_reads_.erase(txn_id);
    rw_in_.erase(txn_id);
    rw_out_.erase(txn_id);
    for (auto &entry : rw_in_) {
        entry.second.erase(txn_id);
    }
    for (auto &entry : rw_out_) {
        entry.second.erase(txn_id);
    }
    for (auto it = rw_edges_.begin(); it != rw_edges_.end(); ) {
        if (it->first == txn_id || it->second == txn_id) {
            it = rw_edges_.erase(it);
        } else {
            ++it;
        }
    }
    ser_txn_start_ts_.erase(txn_id);
    ser_txn_finish_ts_.erase(txn_id);
}

/**
 * @description: 事务的提交方法
 * @param {Transaction*} txn 需要提交的事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
void TransactionManager::commit(Transaction* txn, LogManager* log_manager) {
    // Todo:
    // 1. 如果存在未提交的写操作，提交所有的写操作
    // 2. 释放所有锁
    // 3. 释放事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态
    // 如果需要支持MVCC请在上述过程中添加代码
    if (txn == nullptr) return;

    if (txn->is_mvcc()) {
        timestamp_t commit_ts;
        {
            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            commit_ts = last_commit_ts_.load() + 1;
            txn->set_commit_ts(commit_ts);
            for (auto &key : txn->mvcc_write_keys()) {
                auto it = mvcc_versions_.find(key);
                if (it == mvcc_versions_.end()) continue;
                auto &entry = it->second;
                if (entry.writer_txn == txn->get_transaction_id()) {
                    entry.writer_txn = INVALID_TXN_ID;
                    entry.commit_ts = commit_ts;
                    entry.rollback_delete_physical = false;
                    entry.rollback_restore_physical = false;
                    entry.restore_snapshot = nullptr;
                    entry.had_entry_on_abort = false;
                }
            }
            if (txn->get_isolation_level() == IsolationLevel::SERIALIZABLE) {
                ser_txn_finish_ts_[txn->get_transaction_id()] = commit_ts;
            }
            last_commit_ts_.store(commit_ts);
        }
        running_txns_.UpdateCommitTs(commit_ts);
        running_txns_.RemoveTxn(txn->get_read_ts());
        {
            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            CleanupSerializableStateLocked();
        }
    }

    auto write_set = txn->get_write_set();
    while (!write_set->empty()) {
        delete write_set->back();
        write_set->pop_back();
    }

    // 释放所有锁
    auto lock_set = txn->get_lock_set();
    for (auto &lock_id : *lock_set) {
        lock_manager_->unlock(txn, lock_id);
    }
    lock_set->clear();

    // 更新事务状态
    txn->set_state(TransactionState::COMMITTED);
}

/**
 * @description: 事务的终止（回滚）方法
 * @param {Transaction *} txn 需要回滚的事务
 * @param {LogManager} *log_manager 日志管理器指针
 */
void TransactionManager::abort(Transaction * txn, LogManager *log_manager) {
    // Todo:
    // 1. 回滚所有写操作
    // 2. 释放所有锁
    // 3. 清空事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态
    // 如果需要支持MVCC请在上述过程中添加代码
    if (txn == nullptr) return;

    if (txn->is_mvcc()) {
        std::vector<WriteRecord *> records;
        auto write_set = txn->get_write_set();
        while (!write_set->empty()) {
            records.push_back(write_set->back());
            write_set->pop_back();
        }

        for (auto *write_record : records) {
            const std::string &tab_name = write_record->GetTableName();
            TabMeta &tab = sm_manager_->db_.get_table(tab_name);
            RmFileHandle *fh = sm_manager_->fhs_.at(tab_name).get();
            Rid rid = write_record->GetRid();
            std::string key = MvccKey(tab_name, rid);

            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            auto it = mvcc_versions_.find(key);
            if (it == mvcc_versions_.end() || it->second.writer_txn != txn->get_transaction_id()) {
                delete write_record;
                continue;
            }
            auto &entry = it->second;
            if (entry.rollback_delete_physical) {
                if (fh->is_record(rid)) {
                    auto rec = fh->get_record(rid, nullptr);
                    delete_index_entries(sm_manager_, tab_name, tab, *rec, txn);
                    fh->delete_record(rid, nullptr);
                }
            } else if (entry.rollback_restore_physical) {
                bool restore_deleted = entry.restore_snapshot != nullptr && entry.restore_snapshot->is_deleted;
                if (fh->is_record(rid)) {
                    auto curr_rec = fh->get_record(rid, nullptr);
                    delete_index_entries(sm_manager_, tab_name, tab, *curr_rec, txn);
                    fh->update_record(rid, entry.rollback_record.data, nullptr);
                } else {
                    fh->insert_record(rid, entry.rollback_record.data);
                }
                if (!restore_deleted) {
                    insert_index_entries(sm_manager_, tab_name, tab, entry.rollback_record, rid, txn);
                }
            }
            if (entry.had_entry_on_abort && entry.restore_snapshot != nullptr) {
                mvcc_versions_[key] = *entry.restore_snapshot;
            } else {
                mvcc_versions_.erase(it);
            }
            delete write_record;
        }

        {
            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            ClearTxnStateLocked(txn);
        }
	        running_txns_.RemoveTxn(txn->get_read_ts());
	        {
	            std::scoped_lock<std::mutex> lock(mvcc_latch_);
	            CleanupSerializableStateLocked();
	        }

        auto lock_set = txn->get_lock_set();
        for (auto &lock_id : *lock_set) {
            lock_manager_->unlock(txn, lock_id);
        }
        lock_set->clear();

        txn->set_state(TransactionState::ABORTED);
        return;
    }

    auto write_set = txn->get_write_set();
    while (!write_set->empty()) {
        WriteRecord *write_record = write_set->back();
        write_set->pop_back();

        const std::string &tab_name = write_record->GetTableName();
        TabMeta &tab = sm_manager_->db_.get_table(tab_name);
        RmFileHandle *fh = sm_manager_->fhs_.at(tab_name).get();
        Rid rid = write_record->GetRid();

        switch (write_record->GetWriteType()) {
            case WType::INSERT_TUPLE: {
                auto rec = fh->get_record(rid, nullptr);
                delete_index_entries(sm_manager_, tab_name, tab, *rec, txn);
                fh->delete_record(rid, nullptr);
                break;
            }
            case WType::DELETE_TUPLE: {
                RmRecord &old_rec = write_record->GetRecord();
                fh->insert_record(rid, old_rec.data);
                insert_index_entries(sm_manager_, tab_name, tab, old_rec, rid, txn);
                break;
            }
            case WType::UPDATE_TUPLE: {
                auto curr_rec = fh->get_record(rid, nullptr);
                delete_index_entries(sm_manager_, tab_name, tab, *curr_rec, txn);

                RmRecord &old_rec = write_record->GetRecord();
                fh->update_record(rid, old_rec.data, nullptr);
                insert_index_entries(sm_manager_, tab_name, tab, old_rec, rid, txn);
                break;
            }
        }

        delete write_record;
    }

    // 释放所有锁
    auto lock_set = txn->get_lock_set();
    for (auto &lock_id : *lock_set) {
        lock_manager_->unlock(txn, lock_id);
    }
    lock_set->clear();

    // 更新事务状态
    txn->set_state(TransactionState::ABORTED);
}
