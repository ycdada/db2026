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
#include <unordered_set>

std::unordered_map<txn_id_t, Transaction *> TransactionManager::txn_map = {};

namespace {

constexpr size_t TXN_POOL_GRACE_SIZE = 64;

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

bool index_keys_changed(const TabMeta &tab, const RmRecord &old_rec, const RmRecord &new_rec) {
    for (auto &index : tab.indexes) {
        for (int i = 0; i < index.col_num; ++i) {
            auto &col = index.cols[i];
            if (memcmp(old_rec.data + col.offset, new_rec.data + col.offset, col.len) != 0) {
                return true;
            }
        }
    }
    return false;
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

TransactionManager::~TransactionManager() {
    for (auto &entry : reusable_txns_) {
        for (auto *txn : entry.second) {
            delete txn;
        }
    }
    reusable_txns_.clear();
    for (auto &entry : txn_map) {
        delete entry.second;
    }
    txn_map.clear();
}

/**
 * @description: 事务的开始方法
 * @return {Transaction*} 开始事务的指针
 * @param {Transaction*} txn 事务指针，空指针代表需要创建新事务，否则开始已有事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
Transaction *TransactionManager::begin(Transaction *txn, LogManager *log_manager, IsolationLevel isolation_level,
                                       bool track_in_map) {
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
        std::unique_lock<std::mutex> lock(latch_);
        auto &thread_pool = reusable_txns_[std::this_thread::get_id()];
        if (thread_pool.size() > TXN_POOL_GRACE_SIZE) {
            txn = thread_pool.front();
            thread_pool.pop_front();
            txn->ResetForReuse(new_txn_id, isolation_level);
        } else {
            txn = new Transaction(new_txn_id, isolation_level);
        }
        txn->set_mvcc_enabled(isolation_level == IsolationLevel::SNAPSHOT_ISOLATION ||
                              isolation_level == IsolationLevel::SERIALIZABLE);
        txn->set_tracked_in_map(track_in_map);
        txn->set_state(TransactionState::DEFAULT);
        if (track_in_map) {
            txn_map[new_txn_id] = txn;
        }
    }
    // 如果是已有事务，直接复用
    txn->set_state(TransactionState::GROWING);
    if (new_txn && txn->is_mvcc()) {
        timestamp_t start_ts = last_commit_ts_.load();
        txn->set_start_ts(start_ts);
        txn->set_read_ts(start_ts);
        running_txns_.AddTxn(start_ts);
        active_mvcc_txn_count_.fetch_add(1);
    }
    return txn;
}

void TransactionManager::release_transaction(Transaction *txn) {
    if (txn == nullptr) {
        return;
    }
    auto state = txn->get_state();
    if (state != TransactionState::COMMITTED && state != TransactionState::ABORTED) {
        return;
    }
    if (!txn->get_lock_set()->empty() ||
        !txn->get_index_latch_page_set()->empty() || !txn->get_index_deleted_page_set()->empty()) {
        return;
    }
    bool can_reuse = !txn->get_txn_mode() && txn->get_write_set()->empty();

    if (!txn->is_tracked_in_map() && !can_reuse) {
        delete txn;
        return;
    }

    std::unique_lock<std::mutex> lock(latch_);
    if (txn->is_tracked_in_map()) {
        auto it = txn_map.find(txn->get_transaction_id());
        if (it == txn_map.end() || it->second != txn) {
            return;
        }
        txn_map.erase(it);
    }
    if (can_reuse) {
        txn->ClearCompletedState();
        reusable_txns_[std::this_thread::get_id()].push_back(txn);
    } else {
        lock.unlock();
        delete txn;
    }
}

bool TransactionManager::IsMvccTxn(Transaction *txn) const {
    return txn != nullptr && txn->is_mvcc();
}

bool TransactionManager::ShouldVersionWrites(Transaction *txn) const {
    if (txn == nullptr) return false;
    // 写者自身是 SI/SER，或系统中存在活跃 SI/SER 事务时，必须保留旧版本。
    return txn->is_mvcc() || active_mvcc_txn_count_.load() > 0;
}

bool TransactionManager::HasActiveMvccTransactions() const {
    return active_mvcc_txn_count_.load() > 0;
}

bool TransactionManager::HasMvccState() const {
    return active_mvcc_txn_count_.load(std::memory_order_acquire) > 0 ||
           mvcc_entry_count_.load(std::memory_order_acquire) > 0;
}

std::string TransactionManager::MvccKey(const std::string &tab_name, const Rid &rid) const {
    return tab_name + "#" + std::to_string(rid.page_no) + "#" + std::to_string(rid.slot_no);
}

TransactionManager::MvccEntry &TransactionManager::EnsureMvccEntryLocked(
    const std::string &tab_name, const Rid &rid, const RmRecord *physical) {
    std::string key = MvccKey(tab_name, rid);
    auto [it, inserted] = mvcc_versions_.try_emplace(key);
    auto &entry = it->second;
    if (inserted) {
        mvcc_entry_count_.fetch_add(1, std::memory_order_release);
        mvcc_table_keys_[tab_name].insert(key);
    }
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
        // RC 读者：先返回自己未提交的写入，否则返回最新已提交版本。
        if (txn != nullptr && entry.writer_txn == txn->get_transaction_id()) {
            if (!entry.exists || entry.is_deleted) return std::nullopt;
            return RmRecord(physical);
        }
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
    if (!IsMvccTxn(txn) && !HasMvccState()) {
        return std::make_unique<RmRecord>(physical);
    }
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

std::unique_ptr<RmRecord> TransactionManager::GetVisibleRecord(
    const std::string &tab_name, const Rid &rid, Transaction *txn,
    const std::function<std::unique_ptr<RmRecord>()> &fetch_physical) {
    if (!IsMvccTxn(txn) && !HasMvccState()) {
        return fetch_physical();
    }
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    auto physical = fetch_physical();
    std::string key = MvccKey(tab_name, rid);
    auto it = mvcc_versions_.find(key);
    if (it == mvcc_versions_.end()) {
        return physical;
    }
    auto visible = VisibleRecordLocked(key, it->second, *physical, txn);
    if (!visible.has_value()) {
        return nullptr;
    }
    return std::make_unique<RmRecord>(*visible);
}

bool TransactionManager::IsVisible(const std::string &tab_name, const Rid &rid,
                                   const RmRecord &physical, Transaction *txn) {
    return GetVisibleRecord(tab_name, rid, physical, txn) != nullptr;
}

TransactionManager::PhysicalInsertKey TransactionManager::MakePhysicalInsertKey(
    const std::string &tab_name, const Rid &rid) const {
    auto fh_it = sm_manager_->fhs_.find(tab_name);
    if (fh_it == sm_manager_->fhs_.end()) {
        throw TableNotFoundError(tab_name);
    }
    return PhysicalInsertKey{fh_it->second->GetFd(), rid.page_no, rid.slot_no};
}

TransactionManager::PhysicalInsertKey TransactionManager::MakePhysicalInsertKey(int fd, const Rid &rid) const {
    return PhysicalInsertKey{fd, rid.page_no, rid.slot_no};
}

void TransactionManager::RegisterPhysicalInsert(const std::string &tab_name, const Rid &rid, Transaction *txn) {
    if (txn == nullptr) {
        return;
    }
    auto key = MakePhysicalInsertKey(tab_name, rid);
    RegisterPhysicalInsert(key.fd, rid, txn);
}

void TransactionManager::RegisterPhysicalInsert(int fd, const Rid &rid, Transaction *txn) {
    if (txn == nullptr) {
        return;
    }
    auto key = MakePhysicalInsertKey(fd, rid);
    std::scoped_lock<std::mutex> lock(physical_insert_latch_);
    physical_insert_owners_[key] = txn->get_transaction_id();
}

void TransactionManager::UnregisterPhysicalInsert(const std::string &tab_name, const Rid &rid, Transaction *txn) {
    auto key = MakePhysicalInsertKey(tab_name, rid);
    UnregisterPhysicalInsert(key.fd, rid, txn);
}

void TransactionManager::UnregisterPhysicalInsert(int fd, const Rid &rid, Transaction *txn) {
    auto key = MakePhysicalInsertKey(fd, rid);
    std::scoped_lock<std::mutex> lock(physical_insert_latch_);
    auto it = physical_insert_owners_.find(key);
    if (it == physical_insert_owners_.end()) {
        return;
    }
    if (txn == nullptr || it->second == txn->get_transaction_id()) {
        physical_insert_owners_.erase(it);
    }
}

bool TransactionManager::OwnsPhysicalInsert(const std::string &tab_name, const Rid &rid, Transaction *txn) {
    if (txn == nullptr) {
        return false;
    }
    auto key = MakePhysicalInsertKey(tab_name, rid);
    return OwnsPhysicalInsert(key.fd, rid, txn);
}

bool TransactionManager::OwnsPhysicalInsert(int fd, const Rid &rid, Transaction *txn) {
    if (txn == nullptr) {
        return false;
    }
    auto key = MakePhysicalInsertKey(fd, rid);
    std::scoped_lock<std::mutex> lock(physical_insert_latch_);
    auto it = physical_insert_owners_.find(key);
    return it != physical_insert_owners_.end() && it->second == txn->get_transaction_id();
}

std::vector<std::pair<Rid, RmRecord>> TransactionManager::CollectVisibleVersionRecords(
    const std::string &tab_name, const std::vector<ColMeta> &cols, const std::vector<Condition> &conds,
    const std::set<std::pair<int, int>> &seen_rids, Transaction *txn) {
    std::vector<std::pair<Rid, RmRecord>> records;
    if (!IsMvccTxn(txn)) {
        return records;
    }

    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    auto table_it = mvcc_index_compensation_keys_.find(tab_name);
    if (table_it == mvcc_index_compensation_keys_.end()) {
        return records;
    }
    for (auto &key : table_it->second) {
        auto entry_it = mvcc_versions_.find(key);
        if (entry_it == mvcc_versions_.end()) {
            continue;
        }
        auto &entry_pair = *entry_it;
        auto &entry = entry_pair.second;
        if (!entry.has_head_record) {
            continue;
        }
        if (seen_rids.count({entry.rid.page_no, entry.rid.slot_no}) != 0) {
            continue;
        }
        auto visible = VisibleRecordLocked(entry_pair.first, entry, entry.head_record, txn);
        if (!visible.has_value() || !eval_txn_conds(visible->data, cols, conds)) {
            continue;
        }
        records.emplace_back(entry.rid, *visible);
    }
    return records;
}

timestamp_t TransactionManager::GetWatermark() {
    return running_txns_.GetWatermark();
}

void TransactionManager::GarbageCollection() {
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    timestamp_t watermark = running_txns_.GetWatermark();
    std::vector<std::string> removable_entries;
    for (auto &entry_pair : mvcc_versions_) {
        auto &entry = entry_pair.second;
        if (entry.writer_txn != INVALID_TXN_ID) {
            continue;
        }

        if (!entry.undo_versions.empty()) {
            int keep_idx = -1;
            for (int i = static_cast<int>(entry.undo_versions.size()) - 1; i >= 0; --i) {
                auto &version = entry.undo_versions[i];
                if (version.writer_txn == INVALID_TXN_ID && version.commit_ts <= watermark) {
                    keep_idx = i;
                    break;
                }
            }
            if (keep_idx >= 0) {
                entry.undo_versions.erase(entry.undo_versions.begin(), entry.undo_versions.begin() + keep_idx);
            }
            if (entry.commit_ts <= watermark) {
                entry.undo_versions.clear();
            }
        }

        if (entry.exists && entry.is_deleted && entry.commit_ts <= watermark) {
            entry.undo_versions.clear();
            removable_entries.push_back(entry_pair.first);
        } else if (entry.exists && !entry.is_deleted && entry.commit_ts <= watermark && entry.undo_versions.empty()) {
            removable_entries.push_back(entry_pair.first);
        }
    }
    for (auto &key : removable_entries) {
        auto it = mvcc_versions_.find(key);
        if (it == mvcc_versions_.end()) {
            continue;
        }
        if (it->second.exists && it->second.is_deleted) {
            auto fh_it = sm_manager_->fhs_.find(it->second.tab_name);
            if (fh_it != sm_manager_->fhs_.end()) {
                try {
                    if (fh_it->second->is_record(it->second.rid)) {
                        fh_it->second->delete_record(it->second.rid, nullptr);
                    }
                } catch (...) {
                }
            }
        }
        auto table_it = mvcc_table_keys_.find(it->second.tab_name);
        if (table_it != mvcc_table_keys_.end()) {
            table_it->second.erase(key);
            if (table_it->second.empty()) {
                mvcc_table_keys_.erase(table_it);
            }
        }
        auto compensation_it = mvcc_index_compensation_keys_.find(it->second.tab_name);
        if (compensation_it != mvcc_index_compensation_keys_.end()) {
            compensation_it->second.erase(key);
            if (compensation_it->second.empty()) {
                mvcc_index_compensation_keys_.erase(compensation_it);
            }
        }
        mvcc_versions_.erase(it);
        mvcc_entry_count_.fetch_sub(1, std::memory_order_release);
    }
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

    // 写写冲突 abort 仅对 SI/SER 写者生效（mirror SI WW rules）。RC 写者只保留旧版本、不 abort，
    // 否则会破坏题目9 示例二（T2 未 SET 必须成功提交，同时保留 T1 的快照）。
    if (IsMvccTxn(txn)) {
        if (entry.writer_txn != INVALID_TXN_ID && entry.writer_txn != txn_id) {
            throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
        }
        if (entry.writer_txn != txn_id && entry.commit_ts > txn->get_start_ts()) {
            throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
        }
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
    for (txn_id_t tin : in->second) {
        for (txn_id_t tout : out->second) {
            if (tin == tout) {
                return true;
            }
            auto tout_finish = ser_txn_finish_ts_.find(tout);
            if (tout_finish == ser_txn_finish_ts_.end()) {
                continue;
            }
            auto tin_finish = ser_txn_finish_ts_.find(tin);
            if (tin_finish == ser_txn_finish_ts_.end() || tout_finish->second < tin_finish->second) {
                return true;
            }
        }
    }
    return false;
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
    if (!ShouldVersionWrites(txn)) return;
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    MvccInsertLocked(tab_name, rid, new_rec, txn);
}

std::vector<std::string> TransactionManager::BuildMvccConflictKeys(const TabMeta &tab,
                                                                    const RmRecord &record,
                                                                    bool include_fallback) const {
    std::vector<std::string> keys;
    keys.reserve(std::max<size_t>(1, tab.indexes.size()));
    for (auto &index : tab.indexes) {
        auto raw_key = make_index_key(record, index);
        std::string key = "idx:";
        for (auto &col : index.cols) {
            key += col.name;
            key += ",";
        }
        key.push_back('\0');
        key.append(raw_key.data(), raw_key.size());
        keys.emplace_back(std::move(key));
    }
    if (keys.empty() && include_fallback && !tab.cols.empty()) {
        auto &col = tab.cols.front();
        std::string key = "fallback:";
        key += col.name;
        key.push_back('\0');
        key.append(record.data + col.offset, col.len);
        keys.emplace_back(std::move(key));
    }
    return keys;
}

bool TransactionManager::RecordsConflictByLogicalKey(const TabMeta &tab, const RmRecord &lhs,
                                                     const RmRecord &rhs, bool include_fallback) const {
    auto lhs_keys = BuildMvccConflictKeys(tab, lhs, include_fallback);
    auto rhs_keys = BuildMvccConflictKeys(tab, rhs, include_fallback);
    for (auto &lhs_key : lhs_keys) {
        for (auto &rhs_key : rhs_keys) {
            if (lhs_key == rhs_key) {
                return true;
            }
        }
    }
    return false;
}

void TransactionManager::CheckMvccUniqueConflict(const std::string &tab_name, const RmRecord &new_rec,
                                                 Transaction *txn, const Rid *self_rid) {
    if (!IsMvccTxn(txn)) return;
    std::scoped_lock<std::mutex> lock(mvcc_latch_);
    const TabMeta &tab = sm_manager_->db_.get_table(tab_name);
    auto table_it = mvcc_table_keys_.find(tab_name);
    if (table_it == mvcc_table_keys_.end()) {
        return;
    }
    for (auto &version_key : table_it->second) {
        auto entry_it = mvcc_versions_.find(version_key);
        if (entry_it == mvcc_versions_.end()) {
            continue;
        }
        auto &entry = entry_it->second;
        if (self_rid != nullptr && entry.rid.page_no == self_rid->page_no && entry.rid.slot_no == self_rid->slot_no) {
            continue;
        }
        if (!entry.has_head_record || entry.last_writer_txn == txn->get_transaction_id()) {
            continue;
        }
        bool conflicts_with_snapshot = entry.writer_txn != INVALID_TXN_ID || entry.commit_ts > txn->get_start_ts();
        if (!conflicts_with_snapshot) {
            continue;
        }
        bool has_live_version = entry.exists && !entry.is_deleted;
        for (auto it = entry.undo_versions.rbegin(); !has_live_version && it != entry.undo_versions.rend(); ++it) {
            has_live_version = !it->is_deleted;
        }
        if (!has_live_version) {
            continue;
        }
        bool include_fallback = self_rid == nullptr;
        if (entry.exists && !entry.is_deleted &&
            RecordsConflictByLogicalKey(tab, new_rec, entry.head_record, include_fallback)) {
            throw TransactionAbortException(txn->get_transaction_id(), AbortReason::MVCC_CONFLICT);
        }
        for (auto &version : entry.undo_versions) {
            if (version.is_deleted) {
                continue;
            }
            if (RecordsConflictByLogicalKey(tab, new_rec, version.record, include_fallback)) {
                throw TransactionAbortException(txn->get_transaction_id(), AbortReason::MVCC_CONFLICT);
            }
        }
    }
}

void TransactionManager::CheckMvccInsertConflict(const std::string &tab_name, const RmRecord &new_rec,
                                                 Transaction *txn) {
    CheckMvccUniqueConflict(tab_name, new_rec, txn, nullptr);
}

Rid TransactionManager::MvccInsertWithPhysical(const std::string &tab_name, const RmRecord &new_rec, Transaction *txn,
                                               const std::function<Rid()> &insert_fn,
                                               const std::function<void(const Rid &)> &rollback_fn) {
    if (!ShouldVersionWrites(txn)) {
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
    if (!ShouldVersionWrites(txn)) return;
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
    const TabMeta &tab = sm_manager_->db_.get_table(tab_name);
    if (index_keys_changed(tab, old_rec, new_rec)) {
        mvcc_index_compensation_keys_[tab_name].insert(MvccKey(tab_name, rid));
    }
}

void TransactionManager::MvccDelete(const std::string &tab_name, const Rid &rid,
                                    const RmRecord &old_rec, Transaction *txn) {
    if (!ShouldVersionWrites(txn)) return;
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
    mvcc_index_compensation_keys_[tab_name].insert(MvccKey(tab_name, rid));
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

    if (txn->is_mvcc() || !txn->mvcc_write_keys().empty()) {
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
        // 仅在 begin 时登记过的 SI/SER 事务才回收活跃计数与水位。
        if (txn->is_mvcc()) {
            running_txns_.RemoveTxn(txn->get_read_ts());
            active_mvcc_txn_count_.fetch_sub(1);
        }
        {
            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            CleanupSerializableStateLocked();
        }
        if ((gc_commit_counter_.fetch_add(1) & 1023U) == 1023U) {
            GarbageCollection();
        }
    }

    auto write_set = txn->get_write_set();
    bool has_writes = !write_set->empty() || !txn->mvcc_write_keys().empty();
    while (!write_set->empty()) {
        WriteRecord *write_record = write_set->back();
        if (write_record->GetWriteType() == WType::INSERT_TUPLE) {
            if (write_record->GetFd() >= 0) {
                UnregisterPhysicalInsert(write_record->GetFd(), write_record->GetRid(), txn);
            } else {
                UnregisterPhysicalInsert(write_record->GetTableName(), write_record->GetRid(), txn);
            }
        }
        delete write_record;
        write_set->pop_back();
    }

    // 释放所有锁
    auto lock_set = txn->get_lock_set();
    std::vector<LockDataId> locks(lock_set->begin(), lock_set->end());
    for (auto &lock_id : locks) {
        lock_manager_->unlock(txn, lock_id);
    }
    lock_set->clear();

    // 更新事务状态
    txn->set_state(TransactionState::COMMITTED);
    if (has_writes && log_manager != nullptr) {
        CommitLogRecord log(txn->get_transaction_id(), txn->get_prev_lsn());
        lsn_t lsn = log_manager->add_log_record(&log);
        txn->set_prev_lsn(lsn);
        log_manager->flush_log_to_disk();
    }
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

    if (txn->is_mvcc() || !txn->mvcc_write_keys().empty()) {
        std::vector<WriteRecord *> records;
        auto write_set = txn->get_write_set();
        while (!write_set->empty()) {
            records.push_back(write_set->back());
            write_set->pop_back();
        }
        bool has_writes = !records.empty() || !txn->mvcc_write_keys().empty();

        std::unordered_set<std::string> processed_keys;
        for (auto *write_record : records) {
            const std::string &tab_name = write_record->GetTableName();
            TabMeta &tab = sm_manager_->db_.get_table(tab_name);
            RmFileHandle *fh = sm_manager_->fhs_.at(tab_name).get();
            Rid rid = write_record->GetRid();
            std::string key = MvccKey(tab_name, rid);
            if (!processed_keys.insert(key).second) {
                if (write_record->GetWriteType() == WType::INSERT_TUPLE) {
                    UnregisterPhysicalInsert(tab_name, rid, txn);
                }
                delete write_record;
                continue;
            }

            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            auto it = mvcc_versions_.find(key);
            if (it == mvcc_versions_.end() || it->second.writer_txn != txn->get_transaction_id()) {
                if (write_record->GetWriteType() == WType::INSERT_TUPLE) {
                    UnregisterPhysicalInsert(tab_name, rid, txn);
                }
                delete write_record;
                continue;
            }
            auto &entry = it->second;
            if (entry.rollback_delete_physical) {
                if (fh->is_record(rid)) {
                    auto rec = fh->get_record(rid, nullptr);
                    delete_index_entries(sm_manager_, tab_name, tab, *rec, txn);
                    fh->delete_record(rid, nullptr);
                    lock_manager_->unlock(txn, LockDataId(fh->GetFd(), rid, LockDataType::RECORD));
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
                auto table_it = mvcc_table_keys_.find(tab_name);
                if (table_it != mvcc_table_keys_.end()) {
                    table_it->second.erase(key);
                    if (table_it->second.empty()) {
                        mvcc_table_keys_.erase(table_it);
                    }
                }
                auto compensation_it = mvcc_index_compensation_keys_.find(tab_name);
                if (compensation_it != mvcc_index_compensation_keys_.end()) {
                    compensation_it->second.erase(key);
                    if (compensation_it->second.empty()) {
                        mvcc_index_compensation_keys_.erase(compensation_it);
                    }
                }
                mvcc_versions_.erase(it);
                mvcc_entry_count_.fetch_sub(1, std::memory_order_release);
            }
            if (write_record->GetWriteType() == WType::INSERT_TUPLE) {
                UnregisterPhysicalInsert(tab_name, rid, txn);
            }
            delete write_record;
        }

        {
            std::scoped_lock<std::mutex> lock(mvcc_latch_);
            ClearTxnStateLocked(txn);
        }
	        // 仅 SI/SER 事务在 begin 登记过活跃计数与水位，RC 写者未登记不可回收。
	        if (txn->is_mvcc()) {
	            running_txns_.RemoveTxn(txn->get_read_ts());
	            active_mvcc_txn_count_.fetch_sub(1);
	        }
	        {
	            std::scoped_lock<std::mutex> lock(mvcc_latch_);
	            CleanupSerializableStateLocked();
	        }

        auto lock_set = txn->get_lock_set();
        std::vector<LockDataId> locks(lock_set->begin(), lock_set->end());
        for (auto &lock_id : locks) {
            lock_manager_->unlock(txn, lock_id);
        }
        lock_set->clear();

        txn->set_state(TransactionState::ABORTED);
        if (has_writes && log_manager != nullptr) {
            AbortLogRecord log(txn->get_transaction_id(), txn->get_prev_lsn());
            lsn_t lsn = log_manager->add_log_record(&log);
            txn->set_prev_lsn(lsn);
            log_manager->flush_log_to_disk();
        }
        return;
    }

    auto write_set = txn->get_write_set();
    bool has_writes = !write_set->empty();
    while (!write_set->empty()) {
        WriteRecord *write_record = write_set->back();
        write_set->pop_back();

        const std::string &tab_name = write_record->GetTableName();
        TabMeta &tab = sm_manager_->db_.get_table(tab_name);
        RmFileHandle *fh = sm_manager_->fhs_.at(tab_name).get();
        Rid rid = write_record->GetRid();

            switch (write_record->GetWriteType()) {
            case WType::INSERT_TUPLE: {
                if (OwnsPhysicalInsert(tab_name, rid, txn)) {
                    auto rec = fh->get_record(rid, nullptr);
                    delete_index_entries(sm_manager_, tab_name, tab, *rec, txn);
                    fh->delete_record(rid, nullptr);
                    lock_manager_->unlock(txn, LockDataId(fh->GetFd(), rid, LockDataType::RECORD));
                }
                UnregisterPhysicalInsert(tab_name, rid, txn);
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
    std::vector<LockDataId> locks(lock_set->begin(), lock_set->end());
    for (auto &lock_id : locks) {
        lock_manager_->unlock(txn, lock_id);
    }
    lock_set->clear();

    // 更新事务状态
    txn->set_state(TransactionState::ABORTED);
    if (has_writes && log_manager != nullptr) {
        AbortLogRecord log(txn->get_transaction_id(), txn->get_prev_lsn());
        lsn_t lsn = log_manager->add_log_record(&log);
        txn->set_prev_lsn(lsn);
        log_manager->flush_log_to_disk();
    }
}
