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

#include <memory>
#include <vector>

#include "execution_defs.h"
#include "common/common.h"
#include "index/ix.h"
#include "system/sm.h"

class AbstractExecutor {
   public:
    Rid _abstract_rid;

    Context *context_;

    // 题目四：运行时行数统计（EXPLAIN ANALYZE 使用），普通执行路径不读取
    size_t rows_ = 0;

    virtual ~AbstractExecutor() = default;

    virtual size_t tupleLen() const { return 0; };

    virtual const std::vector<ColMeta> &cols() const {
        std::vector<ColMeta> *_cols = nullptr;
        return *_cols;
    };

    virtual std::string getType() { return "AbstractExecutor"; };

    // 题目四：输出 EXPLAIN 计划树到 out（depth 为缩进层数），默认空实现
    virtual void explain_print(int depth, std::string &out) {}

    // 题目四：收集本子树覆盖的真实表名（供 Join 节点输出 tables 列表）
    virtual void collect_tables(std::vector<std::string> &out) {}

    virtual void beginTuple(){};

    virtual void nextTuple(){};

    virtual bool is_end() const { return true; };

    virtual void finish() {}

    virtual Rid &rid() = 0;

    virtual std::unique_ptr<RmRecord> Next() = 0;

    virtual size_t NextBatch(std::vector<std::unique_ptr<RmRecord>> &batch, size_t max_batch_size) {
        batch.clear();
        while (batch.size() < max_batch_size && !is_end()) {
            auto rec = Next();
            if (rec != nullptr) {
                batch.push_back(std::move(rec));
            }
            nextTuple();
        }
        return batch.size();
    }

    virtual bool bind_join_key(const RmRecord &, const std::vector<ColMeta> &) { return false; }

    virtual ColMeta get_col_offset(const TabCol &target) { return ColMeta();};

    std::vector<ColMeta>::const_iterator get_col(const std::vector<ColMeta> &rec_cols, const TabCol &target) {
        auto pos = std::find_if(rec_cols.begin(), rec_cols.end(), [&](const ColMeta &col) {
            return col.tab_name == target.tab_name && col.name == target.col_name;
        });
        if (pos == rec_cols.end()) {
            throw ColumnNotFoundError(target.tab_name + '.' + target.col_name);
        }
        return pos;
    }
};
