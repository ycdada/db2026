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

#include <vector>
#include <optional>
#include <cstring>
#include <string>

#include "transaction/transaction.h"
#include "transaction/transaction_manager.h"
#include "common/common.h"

auto ReconstructTuple(const TabMeta *schema, const RmRecord &base_tuple, const TupleMeta &base_meta,
                      const std::vector<UndoLog> &undo_logs) -> std::optional<RmRecord>;


auto IsWriteWriteConflict(timestamp_t tuple_ts, Transaction *txn) -> bool;

// 条件求值辅助函数：判断一条记录是否满足所有条件（AND语义）
inline bool eval_conds(const char *rec_data, const std::vector<ColMeta> &cols,
                        const std::vector<Condition> &conds) {
    for (auto &cond : conds) {
        // 查找左列元数据
        const ColMeta *lhs_meta = nullptr;
        for (auto &col : cols) {
            if (col.tab_name == cond.lhs_col.tab_name && col.name == cond.lhs_col.col_name) {
                lhs_meta = &col;
                break;
            }
        }
        if (!lhs_meta) return false;

        const char *lhs_data = rec_data + lhs_meta->offset;
        ColType type = lhs_meta->type;

        if (cond.is_rhs_val) {
            const Value &rhs = cond.rhs_val;
            if (type == TYPE_INT) {
                int lhs_val = *(const int *)lhs_data;
                int rhs_val = rhs.int_val;
                switch (cond.op) {
                    case OP_EQ: if (!(lhs_val == rhs_val)) return false; break;
                    case OP_NE: if (!(lhs_val != rhs_val)) return false; break;
                    case OP_LT: if (!(lhs_val < rhs_val))  return false; break;
                    case OP_GT: if (!(lhs_val > rhs_val))  return false; break;
                    case OP_LE: if (!(lhs_val <= rhs_val)) return false; break;
                    case OP_GE: if (!(lhs_val >= rhs_val)) return false; break;
                }
            } else if (type == TYPE_FLOAT) {
                float lhs_val = *(const float *)lhs_data;
                float rhs_val = rhs.float_val;
                switch (cond.op) {
                    case OP_EQ: if (!(lhs_val == rhs_val)) return false; break;
                    case OP_NE: if (!(lhs_val != rhs_val)) return false; break;
                    case OP_LT: if (!(lhs_val < rhs_val))  return false; break;
                    case OP_GT: if (!(lhs_val > rhs_val))  return false; break;
                    case OP_LE: if (!(lhs_val <= rhs_val)) return false; break;
                    case OP_GE: if (!(lhs_val >= rhs_val)) return false; break;
                }
            } else if (type == TYPE_STRING) {
                std::string lhs_str(lhs_data, lhs_meta->len);
                lhs_str.resize(strlen(lhs_str.c_str()));
                std::string rhs_str = rhs.str_val;
                switch (cond.op) {
                    case OP_EQ: if (!(lhs_str == rhs_str)) return false; break;
                    case OP_NE: if (!(lhs_str != rhs_str)) return false; break;
                    case OP_LT: if (!(lhs_str < rhs_str))  return false; break;
                    case OP_GT: if (!(lhs_str > rhs_str))  return false; break;
                    case OP_LE: if (!(lhs_str <= rhs_str)) return false; break;
                    case OP_GE: if (!(lhs_str >= rhs_str)) return false; break;
                }
            }
        } else {
            // 右值为列
            const ColMeta *rhs_meta = nullptr;
            for (auto &col : cols) {
                if (col.tab_name == cond.rhs_col.tab_name && col.name == cond.rhs_col.col_name) {
                    rhs_meta = &col;
                    break;
                }
            }
            if (!rhs_meta) return false;

            const char *rhs_data = rec_data + rhs_meta->offset;
            if (type == TYPE_INT) {
                int lhs_val = *(const int *)lhs_data;
                int rhs_val = *(const int *)rhs_data;
                switch (cond.op) {
                    case OP_EQ: if (!(lhs_val == rhs_val)) return false; break;
                    case OP_NE: if (!(lhs_val != rhs_val)) return false; break;
                    case OP_LT: if (!(lhs_val < rhs_val))  return false; break;
                    case OP_GT: if (!(lhs_val > rhs_val))  return false; break;
                    case OP_LE: if (!(lhs_val <= rhs_val)) return false; break;
                    case OP_GE: if (!(lhs_val >= rhs_val)) return false; break;
                }
            } else if (type == TYPE_FLOAT) {
                float lhs_val = *(const float *)lhs_data;
                float rhs_val = *(const float *)rhs_data;
                switch (cond.op) {
                    case OP_EQ: if (!(lhs_val == rhs_val)) return false; break;
                    case OP_NE: if (!(lhs_val != rhs_val)) return false; break;
                    case OP_LT: if (!(lhs_val < rhs_val))  return false; break;
                    case OP_GT: if (!(lhs_val > rhs_val))  return false; break;
                    case OP_LE: if (!(lhs_val <= rhs_val)) return false; break;
                    case OP_GE: if (!(lhs_val >= rhs_val)) return false; break;
                }
            } else if (type == TYPE_STRING) {
                std::string lhs_str(lhs_data, lhs_meta->len);
                lhs_str.resize(strlen(lhs_str.c_str()));
                std::string rhs_str(rhs_data, rhs_meta->len);
                rhs_str.resize(strlen(rhs_str.c_str()));
                switch (cond.op) {
                    case OP_EQ: if (!(lhs_str == rhs_str)) return false; break;
                    case OP_NE: if (!(lhs_str != rhs_str)) return false; break;
                    case OP_LT: if (!(lhs_str < rhs_str))  return false; break;
                    case OP_GT: if (!(lhs_str > rhs_str))  return false; break;
                    case OP_LE: if (!(lhs_str <= rhs_str)) return false; break;
                    case OP_GE: if (!(lhs_str >= rhs_str)) return false; break;
                }
            }
        }
    }
    return true;
}
