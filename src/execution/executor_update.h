/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once
#include "execution_defs.h"
#include "execution_manager.h"
#include "execution_common.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"

class UpdateExecutor : public AbstractExecutor {
   private:
    TabMeta tab_;
    std::vector<Condition> conds_;
    RmFileHandle *fh_;
    std::vector<Rid> rids_;
    std::string tab_name_;
    std::vector<SetClause> set_clauses_;
    SmManager *sm_manager_;

    [[noreturn]] void abort_mvcc_statement() {
        auto txn = context_ != nullptr ? context_->txn_ : nullptr;
        txn_id_t txn_id = txn != nullptr ? txn->get_transaction_id() : INVALID_TXN_ID;
        if (context_ != nullptr && context_->txn_mgr_ != nullptr && context_->txn_mgr_->IsMvccTxn(txn) &&
            txn != nullptr && txn->get_state() != TransactionState::COMMITTED &&
            txn->get_state() != TransactionState::ABORTED) {
            context_->txn_mgr_->abort(txn, context_->log_mgr_);
        }
        throw TransactionAbortException(txn_id, AbortReason::MVCC_CONFLICT);
    }

   public:
    UpdateExecutor(SmManager *sm_manager, const std::string &tab_name, std::vector<SetClause> set_clauses,
                   std::vector<Condition> conds, std::vector<Rid> rids, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = tab_name;
        set_clauses_ = set_clauses;
        tab_ = sm_manager_->db_.get_table(tab_name);
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        conds_ = conds;
        rids_ = rids;
        context_ = context;
    }
    std::unique_ptr<RmRecord> Next() override {
        for (auto &rid : rids_) {
            // 获取当前记录
            bool mvcc = context_ != nullptr && context_->txn_mgr_ != nullptr &&
                        context_->txn_mgr_->IsMvccTxn(context_->txn_);
            // RC 写者在有活跃 SI/SER 事务时也要保留旧版本（题目9 示例二）。
            bool version_writes = context_ != nullptr && context_->txn_mgr_ != nullptr &&
                                  context_->txn_mgr_->ShouldVersionWrites(context_->txn_);
            bool use_2pl_locks = context_ != nullptr && context_->txn_ != nullptr && context_->lock_mgr_ != nullptr &&
                                 (context_->txn_mgr_ == nullptr ||
                                  !context_->txn_mgr_->IsMvccTxn(context_->txn_));
            if (use_2pl_locks) {
                context_->lock_mgr_->lock_exclusive_on_record(context_->txn_, rid, fh_->GetFd());
            }
            std::unique_ptr<RmRecord> old_rec;
            try {
                if (context_ != nullptr && context_->txn_mgr_ != nullptr) {
                    old_rec = context_->txn_mgr_->GetVisibleRecord(
                        tab_name_, rid, context_->txn_,
                        [&]() { return fh_->get_record(rid, context_); });
                } else {
                    old_rec = fh_->get_record(rid, context_);
                }
            } catch (const RecordNotFoundError &) {
                continue;
            }
            if (mvcc) {
                if (old_rec == nullptr) continue;
                if (!eval_conds(old_rec->data, tab_.cols, conds_)) continue;
            } else if (old_rec == nullptr) {
                continue;
            }
            if (!eval_conds(old_rec->data, tab_.cols, conds_)) {
                continue;
            }
            auto new_rec = std::make_unique<RmRecord>(fh_->get_file_hdr().record_size, old_rec->data);

            for (auto &set_clause : set_clauses_) {
                const ColMeta *col_meta = nullptr;
                for (auto &col : tab_.cols) {
                    if (col.name == set_clause.lhs.col_name) {
                        col_meta = &col;
                        break;
                    }
                }
                if (!col_meta) continue;
                if (!set_clause.is_rhs_expr) {
                    memcpy(new_rec->data + col_meta->offset, set_clause.rhs.raw->data, col_meta->len);
                    continue;
                }
                const ColMeta *rhs_meta = nullptr;
                for (auto &col : tab_.cols) {
                    if (col.name == set_clause.rhs_col.col_name) {
                        rhs_meta = &col;
                        break;
                    }
                }
                if (!rhs_meta) continue;
                if (col_meta->type == TYPE_INT) {
                    int lhs = rhs_meta->type == TYPE_INT
                                  ? *(int *)(old_rec->data + rhs_meta->offset)
                                  : static_cast<int>(*(float *)(old_rec->data + rhs_meta->offset));
                    int rhs = set_clause.rhs.type == TYPE_INT
                                  ? set_clause.rhs.int_val
                                  : static_cast<int>(set_clause.rhs.float_val);
                    int value = set_clause.op == '-' ? lhs - rhs : lhs + rhs;
                    memcpy(new_rec->data + col_meta->offset, &value, sizeof(int));
                } else if (col_meta->type == TYPE_FLOAT) {
                    float lhs = rhs_meta->type == TYPE_INT
                                    ? static_cast<float>(*(int *)(old_rec->data + rhs_meta->offset))
                                    : *(float *)(old_rec->data + rhs_meta->offset);
                    float rhs = set_clause.rhs.type == TYPE_INT
                                    ? static_cast<float>(set_clause.rhs.int_val)
                                    : set_clause.rhs.float_val;
                    float value = set_clause.op == '-' ? lhs - rhs : lhs + rhs;
                    memcpy(new_rec->data + col_meta->offset, &value, sizeof(float));
                }
            }

	            if (mvcc) {
	                context_->txn_mgr_->CheckMvccWriteConflict(tab_name_, rid, *old_rec, context_->txn_);
	                context_->txn_mgr_->CheckMvccUniqueConflict(tab_name_, *new_rec, context_->txn_, &rid);
	            }

            bool write_record_appended = false;
            if (version_writes) {
                context_->txn_mgr_->MvccUpdate(tab_name_, rid, *old_rec, *new_rec, context_->txn_);
                if (context_->txn_ != nullptr) {
                    context_->txn_->append_write_record(new WriteRecord(WType::UPDATE_TUPLE, tab_name_, rid, *old_rec));
                    write_record_appended = true;
                }
            }
            if (context_ != nullptr && context_->txn_ != nullptr && context_->log_mgr_ != nullptr) {
                UpdateLogRecord log(context_->txn_->get_transaction_id(), context_->txn_->get_prev_lsn(),
                                    *old_rec, *new_rec, rid, tab_name_);
                lsn_t lsn = context_->log_mgr_->add_log_record(&log);
                context_->txn_->set_prev_lsn(lsn);
            }

            struct ChangedIndexEntry {
                IxIndexHandle *ih;
                std::vector<char> old_key;
                std::vector<char> new_key;
            };
            std::vector<ChangedIndexEntry> changed_index_entries;
            std::vector<std::pair<IxIndexHandle *, std::vector<char>>> deleted_old_index_entries;
            std::vector<std::pair<IxIndexHandle *, std::vector<char>>> inserted_new_index_entries;
            try {
                for (size_t i = 0; i < tab_.indexes.size(); ++i) {
                    auto &index = tab_.indexes[i];
                    auto ih = sm_manager_->ihs_.at(
                        sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                    std::vector<char> old_key(index.col_tot_len);
                    std::vector<char> new_key(index.col_tot_len);
                    int offset = 0;
                    for (int j = 0; j < index.col_num; ++j) {
                        memcpy(old_key.data() + offset, old_rec->data + index.cols[j].offset, index.cols[j].len);
                        memcpy(new_key.data() + offset, new_rec->data + index.cols[j].offset, index.cols[j].len);
                        offset += index.cols[j].len;
                    }
                    if (old_key != new_key) {
                        changed_index_entries.push_back({ih, std::move(old_key), std::move(new_key)});
                    }
                }

                // 只维护键值实际变化的索引，避免非索引列更新期间短暂移除索引项。
                for (auto &entry : changed_index_entries) {
                    entry.ih->delete_entry(entry.old_key.data(), context_->txn_);
                    deleted_old_index_entries.emplace_back(entry.ih, entry.old_key);
                }

                // 将更新后的记录写回
                fh_->update_record(rid, new_rec->data, context_);

                // 插入新的索引项
                for (auto &entry : changed_index_entries) {
                    if (entry.ih->insert_entry(entry.new_key.data(), rid, context_->txn_) == IX_NO_PAGE) {
                        throw RMDBError("Duplicate key in unique index");
                    }
                    inserted_new_index_entries.emplace_back(entry.ih, entry.new_key);
                }
            } catch (TransactionAbortException &) {
                throw;
            } catch (...) {
                for (auto &entry : inserted_new_index_entries) {
                    entry.first->delete_entry(entry.second.data(), context_->txn_);
                }
                fh_->update_record(rid, old_rec->data, context_);
                for (auto &entry : deleted_old_index_entries) {
                    entry.first->insert_entry(entry.second.data(), rid, context_->txn_);
                }
                if (mvcc) {
                    abort_mvcc_statement();
                }
                throw;
            }
            if (!write_record_appended && context_->txn_ != nullptr) {
                context_->txn_->append_write_record(new WriteRecord(WType::UPDATE_TUPLE, tab_name_, rid, *old_rec));
            }
        }
        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};
