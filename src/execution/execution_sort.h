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
#include <cstring>
#include <memory>
#include <vector>
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"

class SortExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;
    std::vector<OrderByTerm> order_bys_;
    std::vector<ColMeta> cols_;
    size_t len_ = 0;
    std::vector<std::unique_ptr<RmRecord>> tuples_;
    size_t pos_ = 0;

    int compare_col(const RmRecord &a, const RmRecord &b, const ColMeta &col) const {
        const char *lhs = a.data + col.offset;
        const char *rhs = b.data + col.offset;
        if (col.type == TYPE_INT) {
            int lv = *(const int *)lhs;
            int rv = *(const int *)rhs;
            return (lv > rv) - (lv < rv);
        } else if (col.type == TYPE_FLOAT) {
            float lv = *(const float *)lhs;
            float rv = *(const float *)rhs;
            return (lv > rv) - (lv < rv);
        } else {
            std::string lv(lhs, col.len);
            std::string rv(rhs, col.len);
            lv.resize(strlen(lv.c_str()));
            rv.resize(strlen(rv.c_str()));
            return (lv > rv) - (lv < rv);
        }
    }

   public:
    SortExecutor(std::unique_ptr<AbstractExecutor> prev, std::vector<OrderByTerm> order_bys) {
        prev_ = std::move(prev);
        order_bys_ = std::move(order_bys);
        cols_ = prev_->cols();
        len_ = prev_->tupleLen();
    }

    void beginTuple() override { 
        tuples_.clear();
        prev_->beginTuple();
        while (!prev_->is_end()) {
            auto rec = prev_->Next();
            if (rec != nullptr) tuples_.push_back(std::move(rec));
            prev_->nextTuple();
        }
        std::vector<ColMeta> order_cols;
        for (auto &order : order_bys_) {
            order_cols.push_back(*get_col(cols_, order.col));
        }
        std::stable_sort(tuples_.begin(), tuples_.end(),
            [&](const std::unique_ptr<RmRecord> &a, const std::unique_ptr<RmRecord> &b) {
                for (size_t i = 0; i < order_cols.size(); i++) {
                    int cmp = compare_col(*a, *b, order_cols[i]);
                    if (cmp != 0) return order_bys_[i].is_desc ? cmp > 0 : cmp < 0;
                }
                return false;
            });
        pos_ = 0;
    }

    void nextTuple() override {
        if (pos_ < tuples_.size()) pos_++;
    }

    bool is_end() const override { return pos_ >= tuples_.size(); }

    std::unique_ptr<RmRecord> Next() override {
        if (is_end()) return nullptr;
        rows_++;
        return std::make_unique<RmRecord>(*tuples_[pos_]);
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return _abstract_rid; }
};
