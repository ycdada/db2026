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
#include "execution_common.h"
#include "index/ix.h"
#include "system/sm.h"
#include "record/rm_scan.h"

class SeqScanExecutor : public AbstractExecutor {
   private:
    std::string tab_name_;              // 表的名称
    std::vector<Condition> conds_;      // scan的条件
    RmFileHandle *fh_;                  // 表的数据文件句柄
    std::vector<ColMeta> cols_;         // scan后生成的记录的字段
    size_t len_;                        // scan后生成的每条记录的长度
    std::vector<Condition> fed_conds_;  // 同conds_，两个字段相同

    Rid rid_;
    std::unique_ptr<RecScan> scan_;     // table_iterator
    std::unique_ptr<RmRecord> cur_rec_;
    std::vector<Rid> returned_rids_;
    bool ser_read_registered_ = false;

    SmManager *sm_manager_;

    bool use_2pl_locks() const {
        return context_ != nullptr && context_->txn_ != nullptr && context_->lock_mgr_ != nullptr &&
               (context_->txn_mgr_ == nullptr || !context_->txn_mgr_->IsMvccTxn(context_->txn_));
    }

    void register_ser_read_once() {
        if (!ser_read_registered_ && context_ != nullptr && context_->txn_mgr_ != nullptr) {
            context_->txn_mgr_->RegisterSerializableRead(tab_name_, cols_, fed_conds_, returned_rids_, context_->txn_);
            ser_read_registered_ = true;
        }
    }

    bool load_visible_current() {
        Rid current_rid = scan_->rid();
        if (use_2pl_locks()) {
            context_->lock_mgr_->lock_shared_on_record(context_->txn_, current_rid, fh_->GetFd());
        }
        std::unique_ptr<RmRecord> visible;
        try {
            if (context_ != nullptr && context_->txn_mgr_ != nullptr) {
                visible = context_->txn_mgr_->GetVisibleRecord(
                    tab_name_, current_rid, context_->txn_,
                    [&]() { return fh_->get_record(current_rid, context_); });
            } else {
                visible = fh_->get_record(current_rid, context_);
            }
        } catch (const RecordNotFoundError &) {
            return false;
        }
	        if (visible != nullptr && eval_conds(visible->data, cols_, fed_conds_)) {
	            cur_rec_ = std::move(visible);
	            rid_ = current_rid;
	            returned_rids_.push_back(rid_);
	            return true;
	        }
        return false;
    }

	   public:
	    SeqScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = std::move(tab_name);
        conds_ = std::move(conds);
        TabMeta &tab = sm_manager_->db_.get_table(tab_name_);
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab.cols;
        len_ = cols_.back().offset + cols_.back().len;

        context_ = context;

        fed_conds_ = conds_;
    }

    void beginTuple() override {
        scan_ = std::make_unique<RmScan>(fh_);
        returned_rids_.clear();
        ser_read_registered_ = false;
        cur_rec_ = nullptr;
        // 跳过不满足条件的记录
        while (!scan_->is_end()) {
            if (load_visible_current()) {
                return;
            }
            scan_->next();
        }
        register_ser_read_once();
    }

    void nextTuple() override {
        if (scan_ == nullptr) return;
        scan_->next();
        cur_rec_ = nullptr;
        while (!scan_->is_end()) {
            if (load_visible_current()) {
                return;
            }
            scan_->next();
        }
        register_ser_read_once();
    }

    bool is_end() const override {
        return scan_ == nullptr || scan_->is_end();
    }

    void finish() override { register_ser_read_once(); }

    std::unique_ptr<RmRecord> Next() override {
	        if (scan_ == nullptr || scan_->is_end()) return nullptr;
	        rows_++;  // 题目四：统计扫描到的行数
	        return std::make_unique<RmRecord>(*cur_rec_);
	    }

    size_t NextBatch(std::vector<std::unique_ptr<RmRecord>> &batch, size_t max_batch_size) override {
        batch.clear();
        while (batch.size() < max_batch_size && scan_ != nullptr && !scan_->is_end()) {
            if (cur_rec_ != nullptr) {
                rows_++;
                batch.push_back(std::make_unique<RmRecord>(*cur_rec_));
            }
            nextTuple();
        }
        return batch.size();
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return rid_; }

    // 题目四：输出 Scan 节点
    void explain_print(int depth, std::string &out) override {
        out += std::string(depth, '\t');
        out += "Scan(table=" + tab_name_ + ", type=SeqScan, rows=" + std::to_string(rows_) + ")\n";
    }

    // 题目四：叶子节点贡献自身表名
    void collect_tables(std::vector<std::string> &out) override { out.push_back(tab_name_); }
};
