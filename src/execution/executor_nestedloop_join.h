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

class NestedLoopJoinExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> left_;    // 左儿子节点（需要join的表）
    std::unique_ptr<AbstractExecutor> right_;   // 右儿子节点（需要join的表）
    size_t len_;                                // join后获得的每条记录的长度
    std::vector<ColMeta> cols_;                 // join后获得的记录的字段

    std::vector<Condition> fed_conds_;          // join条件
    bool isend;

    // 缓存当前匹配的左、右记录，用于Next()拼接
    std::unique_ptr<RmRecord> left_rec_;
    std::unique_ptr<RmRecord> right_rec_;

    // 前进到下一个匹配的左右记录对
    void advance_to_next_match() {
        right_rec_.reset();
        while (!left_->is_end()) {
            // 在当前左记录下查找匹配的右记录
            while (!right_->is_end()) {
                auto cur_right = right_->Next();
                if (cur_right == nullptr) break;
                // 构造拼接记录以评估条件
                auto joined = std::make_unique<RmRecord>(len_);
                memcpy(joined->data, left_rec_->data, left_->tupleLen());
                memcpy(joined->data + left_->tupleLen(), cur_right->data, right_->tupleLen());
                if (eval_conds(joined->data, cols_, fed_conds_)) {
                    right_rec_ = std::move(cur_right);
                    isend = false;
                    return;
                }
                right_->nextTuple();
            }
            // 右表扫描完毕，前进左表并重启右表
            left_->nextTuple();
            if (!left_->is_end()) {
                left_rec_ = left_->Next();
                right_->bind_join_key(*left_rec_, left_->cols());
                right_->beginTuple();
            }
        }
        isend = true;
    }

   public:
    NestedLoopJoinExecutor(std::unique_ptr<AbstractExecutor> left, std::unique_ptr<AbstractExecutor> right,
                            std::vector<Condition> conds) {
        left_ = std::move(left);
        right_ = std::move(right);
        len_ = left_->tupleLen() + right_->tupleLen();
        cols_ = left_->cols();
        auto right_cols = right_->cols();
        for (auto &col : right_cols) {
            col.offset += left_->tupleLen();
        }

        cols_.insert(cols_.end(), right_cols.begin(), right_cols.end());
        isend = false;
        fed_conds_ = std::move(conds);
    }

    void beginTuple() override {
        left_->beginTuple();
        if (!left_->is_end()) {
            left_rec_ = left_->Next();
            right_->bind_join_key(*left_rec_, left_->cols());
            right_->beginTuple();
            advance_to_next_match();
        } else {
            right_rec_.reset();
            isend = true;
        }
    }

    void nextTuple() override {
        if (isend) return;
        right_->nextTuple();
        advance_to_next_match();
    }

    bool is_end() const override { return isend; }

    void finish() override {
        left_->finish();
        right_->finish();
    }

    std::unique_ptr<RmRecord> Next() override {
        if (isend || left_rec_ == nullptr || right_rec_ == nullptr) return nullptr;
        // 拼接当前匹配的左、右记录
        auto rec = std::make_unique<RmRecord>(len_);
        memcpy(rec->data, left_rec_->data, left_->tupleLen());
        memcpy(rec->data + left_->tupleLen(), right_rec_->data, right_->tupleLen());
        rows_++;  // 题目四：统计连接输出行数
        return rec;
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return _abstract_rid; }

    // 题目四：收集子树覆盖的真实表名
    void collect_tables(std::vector<std::string> &out) override {
        left_->collect_tables(out);
        right_->collect_tables(out);
    }

    // 题目四：输出 Join 节点；先打印左（外表）子树，再打印右（内表）子树
    void explain_print(int depth, std::string &out) override {
        std::vector<std::string> tabs;
        collect_tables(tabs);
        std::sort(tabs.begin(), tabs.end());
        tabs.erase(std::unique(tabs.begin(), tabs.end()), tabs.end());

        std::vector<Condition> sc = fed_conds_;
        std::sort(sc.begin(), sc.end(),
                  [](const Condition &a, const Condition &b) { return a.to_string() < b.to_string(); });

        out += std::string(depth, '\t');
        out += "Join(tables=[";
        for (size_t i = 0; i < tabs.size(); i++) {
            if (i) out += ", ";
            out += tabs[i];
        }
        out += "], condition=[";
        for (size_t i = 0; i < sc.size(); i++) {
            if (i) out += ", ";
            out += sc[i].to_string();
        }
        out += "], rows=" + std::to_string(rows_) + ")\n";
        left_->explain_print(depth + 1, out);
        right_->explain_print(depth + 1, out);
    }
};
