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

#include <array>
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
    struct IntValCond {
        int offset = 0;
        int value = 0;
    };

    struct IntColCond {
        int lhs_offset = 0;
        int rhs_offset = 0;
    };

    struct FloatValCond {
        int offset = 0;
        float value = 0;
    };

    struct FloatColCond {
        int lhs_offset = 0;
        int rhs_offset = 0;
    };

    struct StringValCond {
        int offset = 0;
        int len = 0;
        std::string value;
    };

    struct StringColCond {
        int lhs_offset = 0;
        int lhs_len = 0;
        int rhs_offset = 0;
        int rhs_len = 0;
    };

    bool invalid_ = false;
    size_t cond_count_ = 0;
    std::array<std::vector<IntValCond>, 6> int_vals_;
    std::array<std::vector<IntColCond>, 6> int_cols_;
    std::array<std::vector<FloatValCond>, 6> float_vals_;
    std::array<std::vector<FloatColCond>, 6> float_cols_;
    std::array<std::vector<StringValCond>, 6> string_vals_;
    std::array<std::vector<StringColCond>, 6> string_cols_;

    static size_t op_index(CompOp op) {
        return static_cast<size_t>(op);
    }

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

    static bool fixed_string_equal(const char *data, int len, const std::string &value) {
        if (static_cast<int>(value.size()) > len) {
            return false;
        }
        if (!value.empty() && std::memcmp(data, value.data(), value.size()) != 0) {
            return false;
        }
        return static_cast<int>(value.size()) == len || data[value.size()] == '\0';
    }

    static bool eval_int_vals(const char *rec_data, const std::array<std::vector<IntValCond>, 6> &conds) {
        for (const auto &cond : conds[OP_EQ]) {
            if (*(const int *)(rec_data + cond.offset) != cond.value) return false;
        }
        for (const auto &cond : conds[OP_NE]) {
            if (*(const int *)(rec_data + cond.offset) == cond.value) return false;
        }
        for (const auto &cond : conds[OP_LT]) {
            if (*(const int *)(rec_data + cond.offset) >= cond.value) return false;
        }
        for (const auto &cond : conds[OP_GT]) {
            if (*(const int *)(rec_data + cond.offset) <= cond.value) return false;
        }
        for (const auto &cond : conds[OP_LE]) {
            if (*(const int *)(rec_data + cond.offset) > cond.value) return false;
        }
        for (const auto &cond : conds[OP_GE]) {
            if (*(const int *)(rec_data + cond.offset) < cond.value) return false;
        }
        return true;
    }

    static bool eval_int_cols(const char *rec_data, const std::array<std::vector<IntColCond>, 6> &conds) {
        for (const auto &cond : conds[OP_EQ]) {
            if (*(const int *)(rec_data + cond.lhs_offset) != *(const int *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_NE]) {
            if (*(const int *)(rec_data + cond.lhs_offset) == *(const int *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_LT]) {
            if (*(const int *)(rec_data + cond.lhs_offset) >= *(const int *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_GT]) {
            if (*(const int *)(rec_data + cond.lhs_offset) <= *(const int *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_LE]) {
            if (*(const int *)(rec_data + cond.lhs_offset) > *(const int *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_GE]) {
            if (*(const int *)(rec_data + cond.lhs_offset) < *(const int *)(rec_data + cond.rhs_offset)) return false;
        }
        return true;
    }

    static bool eval_float_vals(const char *rec_data, const std::array<std::vector<FloatValCond>, 6> &conds) {
        for (const auto &cond : conds[OP_EQ]) {
            if (*(const float *)(rec_data + cond.offset) != cond.value) return false;
        }
        for (const auto &cond : conds[OP_NE]) {
            if (*(const float *)(rec_data + cond.offset) == cond.value) return false;
        }
        for (const auto &cond : conds[OP_LT]) {
            if (*(const float *)(rec_data + cond.offset) >= cond.value) return false;
        }
        for (const auto &cond : conds[OP_GT]) {
            if (*(const float *)(rec_data + cond.offset) <= cond.value) return false;
        }
        for (const auto &cond : conds[OP_LE]) {
            if (*(const float *)(rec_data + cond.offset) > cond.value) return false;
        }
        for (const auto &cond : conds[OP_GE]) {
            if (*(const float *)(rec_data + cond.offset) < cond.value) return false;
        }
        return true;
    }

    static bool eval_float_cols(const char *rec_data, const std::array<std::vector<FloatColCond>, 6> &conds) {
        for (const auto &cond : conds[OP_EQ]) {
            if (*(const float *)(rec_data + cond.lhs_offset) != *(const float *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_NE]) {
            if (*(const float *)(rec_data + cond.lhs_offset) == *(const float *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_LT]) {
            if (*(const float *)(rec_data + cond.lhs_offset) >= *(const float *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_GT]) {
            if (*(const float *)(rec_data + cond.lhs_offset) <= *(const float *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_LE]) {
            if (*(const float *)(rec_data + cond.lhs_offset) > *(const float *)(rec_data + cond.rhs_offset)) return false;
        }
        for (const auto &cond : conds[OP_GE]) {
            if (*(const float *)(rec_data + cond.lhs_offset) < *(const float *)(rec_data + cond.rhs_offset)) return false;
        }
        return true;
    }

    static bool eval_string_vals(const char *rec_data, const std::array<std::vector<StringValCond>, 6> &conds) {
        for (const auto &cond : conds[OP_EQ]) {
            if (!fixed_string_equal(rec_data + cond.offset, cond.len, cond.value)) {
                return false;
            }
        }
        for (const auto &cond : conds[OP_NE]) {
            if (fixed_string_equal(rec_data + cond.offset, cond.len, cond.value)) {
                return false;
            }
        }
        for (size_t idx = OP_LT; idx < conds.size(); ++idx) {
            CompOp op = static_cast<CompOp>(idx);
            for (const auto &cond : conds[idx]) {
                if (!compare_string(fixed_string_view(rec_data + cond.offset, cond.len), op,
                                    std::string_view(cond.value))) {
                    return false;
                }
            }
        }
        return true;
    }

    static bool eval_string_cols(const char *rec_data, const std::array<std::vector<StringColCond>, 6> &conds) {
        for (size_t idx = 0; idx < conds.size(); ++idx) {
            CompOp op = static_cast<CompOp>(idx);
            for (const auto &cond : conds[idx]) {
                if (!compare_string(fixed_string_view(rec_data + cond.lhs_offset, cond.lhs_len), op,
                                    fixed_string_view(rec_data + cond.rhs_offset, cond.rhs_len))) {
                    return false;
                }
            }
        }
        return true;
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
        invalid_ = false;
        cond_count_ = 0;
        for (auto &bucket : int_vals_) bucket.clear();
        for (auto &bucket : int_cols_) bucket.clear();
        for (auto &bucket : float_vals_) bucket.clear();
        for (auto &bucket : float_cols_) bucket.clear();
        for (auto &bucket : string_vals_) bucket.clear();
        for (auto &bucket : string_cols_) bucket.clear();
        for (auto &cond : conds) {
            const ColMeta *lhs = find_col(cols, cond.lhs_col);
            if (lhs == nullptr) {
                invalid_ = true;
                continue;
            }
            cond_count_++;
            size_t idx = op_index(cond.op);
            if (cond.is_rhs_val) {
                if (lhs->type == TYPE_INT) {
                    int_vals_[idx].push_back({lhs->offset, cond.rhs_val.int_val});
                } else if (lhs->type == TYPE_FLOAT) {
                    float_vals_[idx].push_back({lhs->offset, cond.rhs_val.float_val});
                } else {
                    string_vals_[idx].push_back({lhs->offset, lhs->len, cond.rhs_val.str_val});
                }
            } else {
                const ColMeta *rhs = find_col(cols, cond.rhs_col);
                if (rhs == nullptr) {
                    invalid_ = true;
                    continue;
                }
                if (lhs->type == TYPE_INT) {
                    int_cols_[idx].push_back({lhs->offset, rhs->offset});
                } else if (lhs->type == TYPE_FLOAT) {
                    float_cols_[idx].push_back({lhs->offset, rhs->offset});
                } else {
                    string_cols_[idx].push_back({lhs->offset, lhs->len, rhs->offset, rhs->len});
                }
            }
        }
    }

    bool empty() const { return cond_count_ == 0 && !invalid_; }

    bool eval(const char *rec_data) const {
        return !invalid_ &&
               eval_int_vals(rec_data, int_vals_) &&
               eval_int_cols(rec_data, int_cols_) &&
               eval_float_vals(rec_data, float_vals_) &&
               eval_float_cols(rec_data, float_cols_) &&
               eval_string_vals(rec_data, string_vals_) &&
               eval_string_cols(rec_data, string_cols_);
    }
};
