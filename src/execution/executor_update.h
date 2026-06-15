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
            auto old_rec = fh_->get_record(rid, context_);
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
	                memcpy(new_rec->data + col_meta->offset, set_clause.rhs.raw->data, col_meta->len);
	            }

	            bool mvcc = context_ != nullptr && context_->txn_mgr_ != nullptr &&
	                        context_->txn_mgr_->IsMvccTxn(context_->txn_);
	            if (mvcc) {
	                context_->txn_mgr_->CheckMvccWriteConflict(tab_name_, rid, *old_rec, context_->txn_);
	            }

	            for (auto &index : tab_.indexes) {
	                auto ih = sm_manager_->ihs_.at(
                    sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                std::vector<char> new_key(index.col_tot_len);
                int offset = 0;
                for (int j = 0; j < index.col_num; ++j) {
                    memcpy(new_key.data() + offset, new_rec->data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                std::vector<Rid> result;
                if (ih->get_value(new_key.data(), &result, context_->txn_)) {
                    bool self = result.size() == 1 && result[0].page_no == rid.page_no && result[0].slot_no == rid.slot_no;
                    if (!self) {
                        throw RMDBError("Duplicate key in unique index");
                    }
                }
            }

	            if (mvcc) {
	                context_->txn_mgr_->MvccUpdate(tab_name_, rid, *old_rec, *new_rec, context_->txn_);
	            }

            // 更新前，删除旧索引项
            for (size_t i = 0; i < tab_.indexes.size(); ++i) {
                auto &index = tab_.indexes[i];
                auto ih = sm_manager_->ihs_.at(
                    sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                std::vector<char> old_key(index.col_tot_len);
                int offset = 0;
                for (int j = 0; j < index.col_num; ++j) {
                    memcpy(old_key.data() + offset, old_rec->data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                ih->delete_entry(old_key.data(), context_->txn_);
            }

            // 将更新后的记录写回
            fh_->update_record(rid, new_rec->data, context_);
            if (context_->txn_ != nullptr) {
                context_->txn_->append_write_record(new WriteRecord(WType::UPDATE_TUPLE, tab_name_, rid, *old_rec));
            }

            // 插入新的索引项
            for (size_t i = 0; i < tab_.indexes.size(); ++i) {
                auto &index = tab_.indexes[i];
                auto ih = sm_manager_->ihs_.at(
                    sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                std::vector<char> new_key(index.col_tot_len);
                int offset = 0;
                for (int j = 0; j < index.col_num; ++j) {
                    memcpy(new_key.data() + offset, new_rec->data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                ih->insert_entry(new_key.data(), rid, context_->txn_);
            }
        }
        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};
