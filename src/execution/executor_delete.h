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

class DeleteExecutor : public AbstractExecutor {
   private:
    TabMeta *tab_;                  // 表的元数据
    std::vector<Condition> conds_;  // delete的条件
    RmFileHandle *fh_;              // 表的数据文件句柄
    std::vector<Rid> rids_;         // 需要删除的记录的位置
    std::string tab_name_;          // 表名称
    SmManager *sm_manager_;
    std::vector<IxIndexHandle *> index_handles_;

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
    DeleteExecutor(SmManager *sm_manager, const std::string &tab_name, std::vector<Condition> conds,
                   std::vector<Rid> rids, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = tab_name;
        tab_ = &sm_manager_->db_.get_table(tab_name);
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        conds_ = std::move(conds);
        rids_ = std::move(rids);
        context_ = context;
        index_handles_.reserve(tab_->indexes.size());
        for (auto &index : tab_->indexes) {
            index_handles_.push_back(
                sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get());
        }
    }

    std::unique_ptr<RmRecord> Next() override {
        // 遍历 rids_，删除每条记录及其索引项
        for (auto &rid : rids_) {
            // 获取记录以用于删除索引
            bool mvcc = context_ != nullptr && context_->txn_mgr_ != nullptr &&
                        context_->txn_mgr_->IsMvccTxn(context_->txn_);
            // RC 写者在有活跃 SI/SER 事务时也要保留旧版本并保留物理槽位（题目9 示例二）。
            bool version_writes = context_ != nullptr && context_->txn_mgr_ != nullptr &&
                                  context_->txn_mgr_->ShouldVersionWrites(context_->txn_);
            bool use_2pl_locks = context_ != nullptr && context_->txn_ != nullptr && context_->lock_mgr_ != nullptr &&
                                 (context_->txn_mgr_ == nullptr ||
                                  !context_->txn_mgr_->IsMvccTxn(context_->txn_));
            if (use_2pl_locks) {
                context_->lock_mgr_->lock_exclusive_on_record(context_->txn_, rid, fh_->GetFd());
            }
            std::unique_ptr<RmRecord> rec;
            try {
                if (context_ != nullptr && context_->txn_mgr_ != nullptr &&
                    context_->txn_mgr_->HasMvccState()) {
                    rec = context_->txn_mgr_->GetVisibleRecord(
                        tab_name_, rid, context_->txn_,
                        [&]() { return fh_->get_record(rid, context_); });
                } else {
                    rec = fh_->get_record(rid, context_);
                }
            } catch (const RecordNotFoundError &) {
                continue;
            }
            if (rec == nullptr) {
                continue;
            }
            if (!eval_conds(rec->data, tab_->cols, conds_)) {
                continue;
            }
            if (mvcc) {
                context_->txn_mgr_->CheckMvccWriteConflict(tab_name_, rid, *rec, context_->txn_);
            }
            bool write_record_appended = false;
            if (version_writes) {
                context_->txn_mgr_->MvccDelete(tab_name_, rid, *rec, context_->txn_);
                if (context_->txn_ != nullptr) {
                    context_->txn_->append_write_record(
                        new WriteRecord(WType::DELETE_TUPLE, tab_name_, fh_->GetFd(), rid, *rec));
                    write_record_appended = true;
                }
            }
            if (context_ != nullptr && context_->txn_ != nullptr && context_->log_mgr_ != nullptr) {
                DeleteLogRecord log(context_->txn_->get_transaction_id(), context_->txn_->get_prev_lsn(),
                                    *rec, rid, tab_name_);
                lsn_t lsn = context_->log_mgr_->add_log_record(&log);
                context_->txn_->set_prev_lsn(lsn);
            }
            std::vector<std::pair<IxIndexHandle *, std::vector<char>>> deleted_index_entries;
            try {
                // 删除索引项
                for (size_t i = 0; i < tab_->indexes.size(); ++i) {
                    auto &index = tab_->indexes[i];
                    auto ih = index_handles_[i];
                    std::vector<char> key(index.col_tot_len);
                    int offset = 0;
                    for (int j = 0; j < index.col_num; ++j) {
                        memcpy(key.data() + offset, rec->data + index.cols[j].offset, index.cols[j].len);
                        offset += index.cols[j].len;
                    }
                    if (ih->delete_entry(key.data(), context_->txn_)) {
                        deleted_index_entries.emplace_back(ih, std::move(key));
                    }
                }
                // MVCC 删除保留物理槽位，旧快照仍可通过版本目录读到旧值。
                if (!version_writes) {
                    fh_->delete_record(rid, context_);
                }
                if (!write_record_appended && context_->txn_ != nullptr) {
                    context_->txn_->append_write_record(
                        new WriteRecord(WType::DELETE_TUPLE, tab_name_, fh_->GetFd(), rid, *rec));
                }
            } catch (TransactionAbortException &) {
                throw;
            } catch (...) {
                for (auto &entry : deleted_index_entries) {
                    entry.first->insert_entry(entry.second.data(), rid, context_->txn_);
                }
                if (mvcc) {
                    abort_mvcc_statement();
                }
                throw;
            }
        }
        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};
