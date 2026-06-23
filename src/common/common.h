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

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "defs.h"
#include "record/rm_defs.h"


struct TabCol {
    std::string tab_name;
    std::string col_name;
    std::string tab_alias;   // 题目四：表别名，仅用于 EXPLAIN 显示；匹配仍用真实 tab_name

    friend bool operator<(const TabCol &x, const TabCol &y) {
        return std::make_pair(x.tab_name, x.col_name) < std::make_pair(y.tab_name, y.col_name);
    }

    // 题目四：显示用表名，有别名优先用别名，否则用真实表名
    std::string get_disp_tab() const { return tab_alias.empty() ? tab_name : tab_alias; }

    // 题目四：EXPLAIN 输出格式 "表名.列名"（表名为别名优先）
    std::string to_string() const { return get_disp_tab() + "." + col_name; }
};

struct Value {
    ColType type;  // type of value
    union {
        int int_val;      // int value
        float float_val;  // float value
    };
    std::string str_val;  // string value

    std::shared_ptr<RmRecord> raw;  // raw record buffer

    void set_int(int int_val_) {
        type = TYPE_INT;
        int_val = int_val_;
    }

    void set_float(float float_val_) {
        type = TYPE_FLOAT;
        float_val = float_val_;
    }

    void set_str(std::string str_val_) {
        type = TYPE_STRING;
        str_val = std::move(str_val_);
    }

    void init_raw(int len) {
        assert(raw == nullptr);
        raw = std::make_shared<RmRecord>(len);
        if (type == TYPE_INT) {
            assert(len == sizeof(int));
            *(int *)(raw->data) = int_val;
        } else if (type == TYPE_FLOAT) {
            assert(len == sizeof(float));
            *(float *)(raw->data) = float_val;
        } else if (type == TYPE_STRING) {
            if (len < (int)str_val.size()) {
                throw StringOverflowError();
            }
            memset(raw->data, 0, len);
            memcpy(raw->data, str_val.c_str(), str_val.size());
        }
    }
};

enum CompOp { OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE };

enum AggFuncType { AGG_COUNT, AGG_MAX, AGG_MIN, AGG_SUM, AGG_AVG };

inline std::string agg_func_name(AggFuncType type) {
    switch (type) {
        case AGG_COUNT: return "COUNT";
        case AGG_MAX: return "MAX";
        case AGG_MIN: return "MIN";
        case AGG_SUM: return "SUM";
        case AGG_AVG: return "AVG";
    }
    return "";
}

struct AggCall {
    AggFuncType type;
    TabCol col;
    bool is_star = false;
    ColType arg_type = TYPE_INT;
    ColType result_type = TYPE_INT;
    int result_len = sizeof(int);

    std::string key() const {
        return agg_func_name(type) + "(" + (is_star ? "*" : col.tab_name + "." + col.col_name) + ")";
    }
};

struct SelectTerm {
    bool is_agg = false;
    TabCol col;
    int agg_idx = -1;
    std::string output_name;
    ColType type = TYPE_INT;
    int len = sizeof(int);
};

enum AggTermKind { AGG_TERM_COL, AGG_TERM_AGG, AGG_TERM_VALUE };

struct AggTerm {
    AggTermKind kind = AGG_TERM_VALUE;
    TabCol col;
    int agg_idx = -1;
    Value val;
    ColType type = TYPE_INT;
    int len = sizeof(int);
};

struct AggHavingCond {
    AggTerm lhs;
    CompOp op;
    AggTerm rhs;
};

struct OrderByTerm {
    TabCol col;
    bool is_desc = false;
};

struct Condition {
    TabCol lhs_col;   // left-hand side column
    CompOp op;        // comparison operator
    bool is_rhs_val;  // true if right-hand side is a value (not a column)
    TabCol rhs_col;   // right-hand side column
    Value rhs_val;    // right-hand side value

    // 题目四：EXPLAIN 显示用，保存类型强转前的原始字面量文本（如 "1000"），避免输出成 "1000.000000"
    std::string rhs_raw;

    // 题目四：比较运算符的字符串形式
    std::string op_to_string() const {
        switch (op) {
            case OP_EQ: return "=";
            case OP_NE: return "!=";
            case OP_LT: return "<";
            case OP_GT: return ">";
            case OP_LE: return "<=";
            case OP_GE: return ">=";
        }
        return "?";
    }

    // 题目四：条件的字符串形式，格式与原始 SQL 保持一致（列名 alias-aware，右值用原始字面量）
    std::string to_string() const {
        std::string res = lhs_col.to_string() + op_to_string();
        if (is_rhs_val) {
            if (!rhs_raw.empty()) {
                res += rhs_raw;
            } else if (rhs_val.type == TYPE_INT) {
                res += std::to_string(rhs_val.int_val);
            } else if (rhs_val.type == TYPE_FLOAT) {
                res += std::to_string(rhs_val.float_val);
            } else {
                res += "'" + rhs_val.str_val + "'";
            }
        } else {
            res += rhs_col.to_string();
        }
        return res;
    }
};

struct SetClause {
    TabCol lhs;
    Value rhs;
    bool is_rhs_expr = false;
    TabCol rhs_col;
    char op = 0;
};
