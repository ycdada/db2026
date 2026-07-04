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
#include <string_view>

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

class ConditionEvaluator {
   private:
    struct BoundCond {
        const ColMeta *lhs = nullptr;
        const ColMeta *rhs_col = nullptr;
        const Value *rhs_val = nullptr;
        CompOp op = OP_EQ;
        bool is_rhs_val = true;
    };

    std::vector<BoundCond> conds_;

    static bool compare_int(int lhs, CompOp op, int rhs) {
        switch (op) {
            case OP_EQ: return lhs == rhs;
            case OP_NE: return lhs != rhs;
            case OP_LT: return lhs < rhs;
            case OP_GT: return lhs > rhs;
            case OP_LE: return lhs <= rhs;
            case OP_GE: return lhs >= rhs;
        }
        return false;
    }

    static bool compare_float(float lhs, CompOp op, float rhs) {
        switch (op) {
            case OP_EQ: return lhs == rhs;
            case OP_NE: return lhs != rhs;
            case OP_LT: return lhs < rhs;
            case OP_GT: return lhs > rhs;
            case OP_LE: return lhs <= rhs;
            case OP_GE: return lhs >= rhs;
        }
        return false;
    }

    static bool compare_string(std::string_view lhs, CompOp op, std::string_view rhs) {
        int cmp = lhs.compare(rhs);
        switch (op) {
            case OP_EQ: return cmp == 0;
            case OP_NE: return cmp != 0;
            case OP_LT: return cmp < 0;
            case OP_GT: return cmp > 0;
            case OP_LE: return cmp <= 0;
            case OP_GE: return cmp >= 0;
        }
        return false;
    }

    static std::string_view fixed_string_view(const char *data, int len) {
        return std::string_view(data, strnlen(data, len));
    }

    static const ColMeta *find_col(const std::vector<ColMeta> &cols, const TabCol &target) {
        for (auto &col : cols) {
            if (col.tab_name == target.tab_name && col.name == target.col_name) {
                return &col;
            }
        }
        return nullptr;
    }

   public:
    ConditionEvaluator() = default;

    ConditionEvaluator(const std::vector<ColMeta> &cols, const std::vector<Condition> &conds) {
        bind(cols, conds);
    }

    void bind(const std::vector<ColMeta> &cols, const std::vector<Condition> &conds) {
        conds_.clear();
        conds_.reserve(conds.size());
        for (auto &cond : conds) {
            BoundCond bound;
            bound.lhs = find_col(cols, cond.lhs_col);
            if (bound.lhs == nullptr) {
                conds_.push_back(bound);
                continue;
            }
            bound.op = cond.op;
            bound.is_rhs_val = cond.is_rhs_val;
            if (cond.is_rhs_val) {
                bound.rhs_val = &cond.rhs_val;
            } else {
                bound.rhs_col = find_col(cols, cond.rhs_col);
            }
            conds_.push_back(bound);
        }
    }

    bool eval(const char *rec_data) const {
        for (auto &cond : conds_) {
            if (cond.lhs == nullptr || (!cond.is_rhs_val && cond.rhs_col == nullptr)) {
                return false;
            }
            const char *lhs_data = rec_data + cond.lhs->offset;
            if (cond.is_rhs_val) {
                if (cond.lhs->type == TYPE_INT) {
                    if (!compare_int(*(const int *)lhs_data, cond.op, cond.rhs_val->int_val)) return false;
                } else if (cond.lhs->type == TYPE_FLOAT) {
                    if (!compare_float(*(const float *)lhs_data, cond.op, cond.rhs_val->float_val)) return false;
                } else {
                    if (!compare_string(fixed_string_view(lhs_data, cond.lhs->len), cond.op,
                                        std::string_view(cond.rhs_val->str_val))) {
                        return false;
                    }
                }
                continue;
            }

            const char *rhs_data = rec_data + cond.rhs_col->offset;
            if (cond.lhs->type == TYPE_INT) {
                if (!compare_int(*(const int *)lhs_data, cond.op, *(const int *)rhs_data)) return false;
            } else if (cond.lhs->type == TYPE_FLOAT) {
                if (!compare_float(*(const float *)lhs_data, cond.op, *(const float *)rhs_data)) return false;
            } else {
                if (!compare_string(fixed_string_view(lhs_data, cond.lhs->len), cond.op,
                                    fixed_string_view(rhs_data, cond.rhs_col->len))) {
                    return false;
                }
            }
        }
        return true;
    }
};
