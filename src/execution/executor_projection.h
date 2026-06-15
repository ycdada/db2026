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
#include "index/ix.h"
#include "system/sm.h"

class ProjectionExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;        // 投影节点的儿子节点
    std::vector<ColMeta> cols_;                     // 需要投影的字段
    size_t len_;                                    // 字段总长度
    std::vector<size_t> sel_idxs_;
    std::vector<TabCol> sel_cols_;                  // 题目四：原始投影列（含别名），用于 EXPLAIN 输出
    bool is_star_ = false;                          // 题目四：是否 SELECT *

   public:
    ProjectionExecutor(std::unique_ptr<AbstractExecutor> prev, const std::vector<TabCol> &sel_cols,
                       bool is_star = false) {
        prev_ = std::move(prev);
        sel_cols_ = sel_cols;
        is_star_ = is_star;

        size_t curr_offset = 0;
        auto &prev_cols = prev_->cols();
        for (auto &sel_col : sel_cols) {
            auto pos = get_col(prev_cols, sel_col);
            sel_idxs_.push_back(pos - prev_cols.begin());
            auto col = *pos;
            col.offset = curr_offset;
            curr_offset += col.len;
            cols_.push_back(col);
        }
        len_ = curr_offset;
    }

    void beginTuple() override { prev_->beginTuple(); }

    void nextTuple() override { prev_->nextTuple(); }

    bool is_end() const override { return prev_->is_end(); }

    void finish() override { prev_->finish(); }

    std::unique_ptr<RmRecord> Next() override {
        if (prev_->is_end()) return nullptr;
        auto prev_rec = prev_->Next();
        if (prev_rec == nullptr) return nullptr;
        // 构造投影后的记录
        auto proj_rec = std::make_unique<RmRecord>(len_);
        for (size_t i = 0; i < sel_idxs_.size(); i++) {
            size_t prev_offset = prev_->cols()[sel_idxs_[i]].offset;
            memcpy(proj_rec->data + cols_[i].offset,
                   prev_rec->data + prev_offset,
                   cols_[i].len);
        }
        rows_++;  // 题目四：统计投影输出行数
        return proj_rec;
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return _abstract_rid; }

    bool bind_join_key(const RmRecord &left_rec, const std::vector<ColMeta> &left_cols) override {
        return prev_->bind_join_key(left_rec, left_cols);
    }

    // 题目四：透传子树表名
    void collect_tables(std::vector<std::string> &out) override { prev_->collect_tables(out); }

    // 题目四：输出 Project 节点（列按字典序，SELECT * 输出 [*]）
    void explain_print(int depth, std::string &out) override {
        out += std::string(depth, '\t');
        out += "Project(columns=[";
        if (is_star_) {
            out += "*";
        } else {
            std::vector<std::string> names;
            for (auto &c : sel_cols_) names.push_back(c.to_string());
            std::sort(names.begin(), names.end());
            for (size_t i = 0; i < names.size(); i++) {
                if (i) out += ", ";
                out += names[i];
            }
        }
        out += "], rows=" + std::to_string(rows_) + ")\n";
        prev_->explain_print(depth + 1, out);
    }
};
