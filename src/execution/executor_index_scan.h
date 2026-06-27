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

#include <algorithm>
#include <limits>
#include <set>

#include "execution_common.h"
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"

class IndexScanExecutor : public AbstractExecutor {
   private:
    std::string tab_name_;                      // 表名称
    TabMeta tab_;                               // 表的元数据
    std::vector<Condition> conds_;              // 扫描条件
    RmFileHandle *fh_;                          // 表的数据文件句柄
    std::vector<ColMeta> cols_;                 // 需要读取的字段
    size_t len_;                                // 选取出来的一条记录的长度
    std::vector<Condition> fed_conds_;          // 扫描条件，和conds_字段相同

    std::vector<std::string> index_col_names_;  // index scan涉及到的索引包含的字段
    IndexMeta index_meta_;                      // index scan涉及到的索引元数据

    Rid rid_;
    std::unique_ptr<RecScan> scan_;
    std::unique_ptr<RmRecord> cur_rec_;
    std::vector<std::pair<Rid, RmRecord>> extra_records_;
    size_t extra_pos_ = 0;
    std::set<std::pair<int, int>> seen_rids_;
    std::vector<Rid> returned_rids_;
    bool ser_read_registered_ = false;
    bool end_ = true;

    SmManager *sm_manager_;
    bool is_join_inner_ = false;
    bool join_key_bound_ = false;
    TabCol join_outer_col_;
    std::string join_inner_col_;
    Value join_key_val_;

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

    void load_mvcc_extra_records() {
        if (context_ != nullptr && context_->txn_mgr_ != nullptr &&
            context_->txn_mgr_->IsMvccTxn(context_->txn_)) {
            extra_records_ = context_->txn_mgr_->CollectVisibleVersionRecords(tab_name_, cols_, fed_conds_,
                                                                              seen_rids_, context_->txn_);
        } else {
            extra_records_.clear();
        }
        extra_pos_ = 0;
    }

    bool load_next_extra() {
        if (extra_pos_ >= extra_records_.size()) {
            return false;
        }
        rid_ = extra_records_[extra_pos_].first;
        cur_rec_ = std::make_unique<RmRecord>(extra_records_[extra_pos_].second);
        returned_rids_.push_back(rid_);
        end_ = false;
        return true;
    }

    bool load_visible_index_current() {
        auto rid = scan_->rid();
        seen_rids_.insert({rid.page_no, rid.slot_no});
        if (use_2pl_locks()) {
            context_->lock_mgr_->lock_shared_on_record(context_->txn_, rid, fh_->GetFd());
        }
        std::unique_ptr<RmRecord> rec;
        try {
            if (context_ != nullptr && context_->txn_mgr_ != nullptr) {
                rec = context_->txn_mgr_->GetVisibleRecord(
                    tab_name_, rid, context_->txn_,
                    [&]() { return fh_->get_record(rid, context_); });
            } else {
                rec = fh_->get_record(rid, context_);
            }
        } catch (const RecordNotFoundError &) {
            return false;
        }
        if (rec != nullptr && eval_conds(rec->data, cols_, fed_conds_)) {
            cur_rec_ = std::move(rec);
            rid_ = rid;
            returned_rids_.push_back(rid_);
            end_ = false;
            return true;
        }
        return false;
    }

    void advance_to_next_match() {
        cur_rec_ = nullptr;
        while (scan_ != nullptr && !scan_->is_end()) {
            if (load_visible_index_current()) {
                return;
            }
            scan_->next();
        }
        load_mvcc_extra_records();
        if (load_next_extra()) {
            return;
        }
        end_ = true;
        register_ser_read_once();
    }

    void fill_min_key(char *key, int offset, const ColMeta &col) {
        if (col.type == TYPE_INT) {
            int value = std::numeric_limits<int>::min();
            memcpy(key + offset, &value, sizeof(int));
        } else if (col.type == TYPE_FLOAT) {
            float value = std::numeric_limits<float>::lowest();
            memcpy(key + offset, &value, sizeof(float));
        } else {
            memset(key + offset, 0, col.len);
        }
    }

    void fill_max_key(char *key, int offset, const ColMeta &col) {
        if (col.type == TYPE_INT) {
            int value = std::numeric_limits<int>::max();
            memcpy(key + offset, &value, sizeof(int));
        } else if (col.type == TYPE_FLOAT) {
            float value = std::numeric_limits<float>::max();
            memcpy(key + offset, &value, sizeof(float));
        } else {
            memset(key + offset, 0xff, col.len);
        }
    }

    std::vector<Condition> col_conds(const std::string &col_name) {
        std::vector<Condition> res;
        for (auto &cond : fed_conds_) {
            if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name_ && cond.lhs_col.col_name == col_name) {
                res.push_back(cond);
            }
        }
        return res;
    }

    const ColMeta &find_col(const std::vector<ColMeta> &cols, const std::string &tab_name, const std::string &col_name) {
        auto pos = std::find_if(cols.begin(), cols.end(), [&](const ColMeta &col) {
            return col.tab_name == tab_name && col.name == col_name;
        });
        if (pos == cols.end()) {
            throw ColumnNotFoundError(tab_name + "." + col_name);
        }
        return *pos;
    }

   public:
    IndexScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, std::vector<std::string> index_col_names,
                    Context *context, bool is_join_inner = false, TabCol join_outer_col = {}, std::string join_inner_col = "") {
        sm_manager_ = sm_manager;
        context_ = context;
        tab_name_ = std::move(tab_name);
        tab_ = sm_manager_->db_.get_table(tab_name_);
        conds_ = std::move(conds);
        // index_no_ = index_no;
        index_col_names_ = index_col_names; 
        index_meta_ = *(tab_.get_index_meta(index_col_names_));
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab_.cols;
        len_ = cols_.back().offset + cols_.back().len;
        is_join_inner_ = is_join_inner;
        join_outer_col_ = std::move(join_outer_col);
        join_inner_col_ = std::move(join_inner_col);
        std::map<CompOp, CompOp> swap_op = {
            {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
        };

        for (auto &cond : conds_) {
            if (cond.lhs_col.tab_name != tab_name_) {
                // lhs is on other table, now rhs must be on this table
                assert(!cond.is_rhs_val && cond.rhs_col.tab_name == tab_name_);
                // swap lhs and rhs
                std::swap(cond.lhs_col, cond.rhs_col);
                cond.op = swap_op.at(cond.op);
            }
        }
        fed_conds_ = conds_;
    }

    void beginTuple() override {
        fed_conds_ = conds_;
        if (is_join_inner_) {
            if (!join_key_bound_) {
                scan_ = nullptr;
                cur_rec_ = nullptr;
                extra_records_.clear();
                extra_pos_ = 0;
                seen_rids_.clear();
                returned_rids_.clear();
                ser_read_registered_ = false;
                end_ = true;
                return;
            }
            Condition cond;
            cond.lhs_col = {.tab_name = tab_name_, .col_name = join_inner_col_};
            cond.op = OP_EQ;
            cond.is_rhs_val = true;
            cond.rhs_val = join_key_val_;
            fed_conds_.push_back(cond);
        }
        returned_rids_.clear();
        ser_read_registered_ = false;
        seen_rids_.clear();
        extra_records_.clear();
        extra_pos_ = 0;
        cur_rec_ = nullptr;
        end_ = true;
        auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index_meta_.cols)).get();
        std::vector<char> lower_key(index_meta_.col_tot_len);
        std::vector<char> upper_key(index_meta_.col_tot_len);
        bool has_lower = false;
        bool has_upper = false;
        bool lower_open = false;
        bool upper_open = false;
        bool stop_prefix = false;
        int offset = 0;

        for (auto &col : index_meta_.cols) {
            auto conds = stop_prefix ? std::vector<Condition>() : col_conds(col.name);
            const Condition *eq = nullptr;
            const Condition *lower = nullptr;
            const Condition *upper = nullptr;
            for (auto &cond : conds) {
                if (cond.op == OP_EQ) {
                    eq = &cond;
                } else if (cond.op == OP_GT || cond.op == OP_GE) {
                    lower = &cond;
                } else if (cond.op == OP_LT || cond.op == OP_LE) {
                    upper = &cond;
                }
            }

            if (eq != nullptr) {
                memcpy(lower_key.data() + offset, eq->rhs_val.raw->data, col.len);
                memcpy(upper_key.data() + offset, eq->rhs_val.raw->data, col.len);
                has_lower = true;
                has_upper = true;
            } else {
                if (lower != nullptr) {
                    memcpy(lower_key.data() + offset, lower->rhs_val.raw->data, col.len);
                    has_lower = true;
                    lower_open = lower->op == OP_GT;
                } else {
                    fill_min_key(lower_key.data(), offset, col);
                }
                if (upper != nullptr) {
                    memcpy(upper_key.data() + offset, upper->rhs_val.raw->data, col.len);
                    has_upper = true;
                    upper_open = upper->op == OP_LT;
                } else {
                    fill_max_key(upper_key.data(), offset, col);
                }
                stop_prefix = true;
            }
            offset += col.len;
        }

        Iid lower = has_lower
                        ? (lower_open ? ih->upper_bound(lower_key.data()) : ih->lower_bound(lower_key.data()))
                        : ih->leaf_begin();
        Iid upper = has_upper
                        ? (upper_open ? ih->lower_bound(upper_key.data()) : ih->upper_bound(upper_key.data()))
                        : ih->leaf_end();

        scan_ = std::make_unique<IxScan>(ih, lower, upper, sm_manager_->get_bpm());
        advance_to_next_match();
    }

    void nextTuple() override {
        if (end_) return;
        if (scan_ != nullptr && !scan_->is_end()) {
            scan_->next();
            advance_to_next_match();
            return;
        }
        if (extra_pos_ < extra_records_.size()) {
            extra_pos_++;
            if (load_next_extra()) {
                return;
            }
        }
        cur_rec_ = nullptr;
        end_ = true;
        register_ser_read_once();
    }

    std::unique_ptr<RmRecord> Next() override {
        if (end_ || cur_rec_ == nullptr) return nullptr;
        rows_++;
        return std::make_unique<RmRecord>(*cur_rec_);
    }

    size_t NextBatch(std::vector<std::unique_ptr<RmRecord>> &batch, size_t max_batch_size) override {
        batch.clear();
        while (batch.size() < max_batch_size && !end_) {
            if (cur_rec_ != nullptr) {
                rows_++;
                batch.push_back(std::make_unique<RmRecord>(*cur_rec_));
            }
            nextTuple();
        }
        return batch.size();
    }

    bool is_end() const override {
        return end_;
    }

    void finish() override { register_ser_read_once(); }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return rid_; }

    bool bind_join_key(const RmRecord &left_rec, const std::vector<ColMeta> &left_cols) override {
        if (!is_join_inner_) return false;
        const ColMeta &outer_col = find_col(left_cols, join_outer_col_.tab_name, join_outer_col_.col_name);
        const ColMeta &inner_col = find_col(cols_, tab_name_, join_inner_col_);
        const char *src = left_rec.data + outer_col.offset;
        if (inner_col.type == TYPE_INT) {
            join_key_val_.set_int(*(const int *)src);
        } else if (inner_col.type == TYPE_FLOAT) {
            join_key_val_.set_float(*(const float *)src);
        } else {
            std::string v(src, outer_col.len);
            v.resize(strlen(v.c_str()));
            join_key_val_.set_str(v);
        }
        join_key_val_.raw = nullptr;
        join_key_val_.init_raw(inner_col.len);
        join_key_bound_ = true;
        return true;
    }

    void explain_print(int depth, std::string &out) override {
        out += std::string(depth, '\t');
        out += "Scan(table=" + tab_name_ + ", type=IndexScan, using_index=(";
        for (size_t i = 0; i < index_col_names_.size(); i++) {
            if (i) out += ", ";
            out += index_col_names_[i];
        }
        out += "), rows=" + std::to_string(rows_) + ")\n";
    }

    void collect_tables(std::vector<std::string> &out) override { out.push_back(tab_name_); }
};
