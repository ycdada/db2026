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
#include <string>
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "execution_common.h"
#include "index/ix.h"
#include "system/sm.h"

// 题目四：过滤节点（选择运算）。从子节点拉取记录，用 eval_conds 过滤，仅输出满足条件的行。
// 主要服务于 EXPLAIN ANALYZE：子节点是“空谓词”的 Scan，过滤在本节点完成，rows 即过滤后输出行数。
class FilterExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;   // 子节点
    std::vector<Condition> conds_;             // 过滤条件
    std::unique_ptr<RmRecord> cur_;            // 当前满足条件的记录缓存
    bool is_end_ = true;

    // 从子节点当前位置起，前进到下一条满足条件的记录
    void find_next_valid() {
        while (!prev_->is_end()) {
            auto rec = prev_->Next();
            if (rec != nullptr && eval_conds(rec->data, prev_->cols(), conds_)) {
                cur_ = std::move(rec);
                is_end_ = false;
                return;
            }
            prev_->nextTuple();
        }
        cur_ = nullptr;
        is_end_ = true;
    }

   public:
    FilterExecutor(std::unique_ptr<AbstractExecutor> prev, std::vector<Condition> conds) {
        prev_ = std::move(prev);
        conds_ = std::move(conds);
    }

    void beginTuple() override {
        prev_->beginTuple();
        find_next_valid();
    }

    void nextTuple() override {
        prev_->nextTuple();
        find_next_valid();
    }

    bool is_end() const override { return is_end_; }

    void finish() override { prev_->finish(); }

    std::unique_ptr<RmRecord> Next() override {
        if (is_end_ || cur_ == nullptr) return nullptr;
        rows_++;  // 题目四：统计过滤后输出行数
        // 返回当前记录的副本，保证多次 Next() 安全
        auto out = std::make_unique<RmRecord>(*cur_);
        return out;
    }

    size_t NextBatch(std::vector<std::unique_ptr<RmRecord>> &batch, size_t max_batch_size) override {
        batch.clear();
        while (batch.size() < max_batch_size && !is_end_) {
            if (cur_ != nullptr) {
                batch.push_back(std::make_unique<RmRecord>(*cur_));
                rows_++;
            }
            nextTuple();
        }
        return batch.size();
    }

    const std::vector<ColMeta> &cols() const override { return prev_->cols(); }

    size_t tupleLen() const override { return prev_->tupleLen(); }

    Rid &rid() override { return prev_->rid(); }

    bool bind_join_key(const RmRecord &left_rec, const std::vector<ColMeta> &left_cols) override {
        return prev_->bind_join_key(left_rec, left_cols);
    }

    // 题目四：透传子树表名
    void collect_tables(std::vector<std::string> &out) override { prev_->collect_tables(out); }

    // 题目四：输出 Filter 节点（条件按字典序排序，列名 alias-aware）
    void explain_print(int depth, std::string &out) override {
        std::vector<Condition> sc = conds_;
        std::sort(sc.begin(), sc.end(),
                  [](const Condition &a, const Condition &b) { return a.to_string() < b.to_string(); });
        out += std::string(depth, '\t');
        out += "Filter(condition=[";
        for (size_t i = 0; i < sc.size(); i++) {
            if (i) out += ", ";
            out += sc[i].to_string();
        }
        out += "], rows=" + std::to_string(rows_) + ")\n";
        prev_->explain_print(depth + 1, out);
    }
};
