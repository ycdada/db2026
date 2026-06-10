/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "analyze.h"

/**
 * @description: 分析器，进行语义分析和查询重写，需要检查不符合语义规定的部分
 * @param {shared_ptr<ast::TreeNode>} parse parser生成的结果集
 * @return {shared_ptr<Query>} Query 
 */
std::shared_ptr<Query> Analyze::do_analyze(std::shared_ptr<ast::TreeNode> parse)
{
    std::shared_ptr<Query> query = std::make_shared<Query>();
    if (auto x = std::dynamic_pointer_cast<ast::ExplainStmt>(parse)) {
        // 题目四：EXPLAIN ANALYZE，分析内层 SELECT 并标记 explain
        query->explain = true;
        analyze_select(x->sel, query);
        // parse 保存内层 SelectStmt，便于规划器按普通 select 流程构建计划
        query->parse = std::move(x->sel);
        return query;
    } else if (auto x = std::dynamic_pointer_cast<ast::SelectStmt>(parse))
    {
        analyze_select(x, query);
    } else if (auto x = std::dynamic_pointer_cast<ast::UpdateStmt>(parse)) {
        // 处理表名
        query->tables.push_back(x->tab_name);

        // 获取表的列元数据，用于SET子句的类型检查和转换
        TabMeta &tab = sm_manager_->db_.get_table(x->tab_name);

        // 处理SET子句
        for (auto &sv_set_clause : x->set_clauses) {
            SetClause set_clause;
            set_clause.lhs.col_name = sv_set_clause->col_name;
            set_clause.lhs.tab_name = x->tab_name;
            set_clause.rhs = convert_sv_value(sv_set_clause->val);
            // 查找列元数据，进行类型转换和init_raw
            auto col_meta = tab.get_col(sv_set_clause->col_name);
            if (col_meta == tab.cols.end()) {
                throw ColumnNotFoundError(sv_set_clause->col_name);
            }
            // INT-FLOAT转换
            if (col_meta->type == TYPE_FLOAT && set_clause.rhs.type == TYPE_INT) {
                set_clause.rhs.set_float((float)set_clause.rhs.int_val);
            } else if (col_meta->type == TYPE_INT && set_clause.rhs.type == TYPE_FLOAT) {
                set_clause.rhs.set_int((int)set_clause.rhs.float_val);
            }
            set_clause.rhs.init_raw(col_meta->len);
            query->set_clauses.push_back(set_clause);
        }

        // 处理WHERE条件
        get_clause(x->conds, query->conds);
        check_clause(query->tables, query->conds);
    } else if (auto x = std::dynamic_pointer_cast<ast::DeleteStmt>(parse)) {
        //处理where条件
        get_clause(x->conds, query->conds);
        check_clause({x->tab_name}, query->conds);        
    } else if (auto x = std::dynamic_pointer_cast<ast::InsertStmt>(parse)) {
        // 处理insert 的values值
        for (auto &sv_val : x->vals) {
            query->values.push_back(convert_sv_value(sv_val));
        }
    } else {
        // do nothing
    }
    query->parse = std::move(parse);
    return query;
}

// 题目四：把字面量转成显示用的原始文本（在类型强转之前调用，保留 SQL 原貌）
static std::string literal_to_raw(const std::shared_ptr<ast::Value> &sv_val) {
    if (auto int_lit = std::dynamic_pointer_cast<ast::IntLit>(sv_val)) {
        return std::to_string(int_lit->val);
    } else if (auto float_lit = std::dynamic_pointer_cast<ast::FloatLit>(sv_val)) {
        return std::to_string(float_lit->val);
    } else if (auto str_lit = std::dynamic_pointer_cast<ast::StringLit>(sv_val)) {
        return "'" + str_lit->val + "'";
    }
    return "";
}

// 题目四：分析 SELECT 主体，支持表别名（FROM customers c）。
// 关键点：匹配/执行一律用真实表名（写入 TabCol.tab_name），别名仅写入 tab_alias 供 EXPLAIN 显示。
void Analyze::analyze_select(const std::shared_ptr<ast::SelectStmt> &x, std::shared_ptr<Query> query) {
    // 处理表名（真实名）
    query->tables = x->tabs;

    // 构建 “任意标识符 -> 真实表名” 与 “真实表名 -> 别名” 两张映射
    std::map<std::string, std::string> name2real;   // 别名或真实名 => 真实名
    std::map<std::string, std::string> real2alias;  // 真实名 => 别名（无则空）
    for (size_t i = 0; i < x->tabs.size(); i++) {
        const std::string &real = x->tabs[i];
        name2real[real] = real;
        std::string alias = (i < x->tab_alias.size()) ? x->tab_alias[i] : std::string();
        if (!alias.empty()) {
            name2real[alias] = real;
            real2alias[real] = alias;
        }
    }

    std::vector<ColMeta> all_cols;
    get_all_cols(query->tables, all_cols);

    // 解析一个列引用：把（可能是别名的）表限定符解析为真实表名，并填好显示用别名
    auto resolve_col = [&](const std::string &tok, const std::string &col_name) -> TabCol {
        TabCol tc;
        tc.col_name = col_name;
        std::string real;
        if (!tok.empty()) {
            auto it = name2real.find(tok);
            if (it == name2real.end()) {
                throw TableNotFoundError(tok);
            }
            real = it->second;
        } else {
            // 未指定表名，根据列名在所有表中推断
            for (auto &col : all_cols) {
                if (col.name == col_name) {
                    if (!real.empty() && real != col.tab_name) {
                        throw AmbiguousColumnError(col_name);
                    }
                    real = col.tab_name;
                }
            }
            if (real.empty()) {
                throw ColumnNotFoundError(col_name);
            }
        }
        tc.tab_name = real;
        auto ait = real2alias.find(real);
        tc.tab_alias = (ait != real2alias.end()) ? ait->second : std::string();
        return tc;
    };

    // 处理投影列
    if (x->cols.empty()) {
        // SELECT * ：展开所有列
        query->is_star = true;
        for (auto &col : all_cols) {
            TabCol sel_col;
            sel_col.tab_name = col.tab_name;
            sel_col.col_name = col.name;
            auto ait = real2alias.find(col.tab_name);
            sel_col.tab_alias = (ait != real2alias.end()) ? ait->second : std::string();
            query->cols.push_back(sel_col);
        }
    } else {
        for (auto &sv_sel_col : x->cols) {
            query->cols.push_back(resolve_col(sv_sel_col->tab_name, sv_sel_col->col_name));
        }
    }

    // 处理 where / JOIN ON 条件（已在语法层合并到 x->conds）
    query->conds.clear();
    for (auto &expr : x->conds) {
        Condition cond;
        cond.lhs_col = resolve_col(expr->lhs->tab_name, expr->lhs->col_name);
        cond.op = convert_sv_comp_op(expr->op);
        if (auto rhs_val = std::dynamic_pointer_cast<ast::Value>(expr->rhs)) {
            cond.is_rhs_val = true;
            cond.rhs_raw = literal_to_raw(rhs_val);  // 在类型强转前保存原始文本
            cond.rhs_val = convert_sv_value(rhs_val);
        } else if (auto rhs_col = std::dynamic_pointer_cast<ast::Col>(expr->rhs)) {
            cond.is_rhs_val = false;
            cond.rhs_col = resolve_col(rhs_col->tab_name, rhs_col->col_name);
        }
        query->conds.push_back(cond);
    }

    // 类型检查与右值 init_raw（与 check_clause 逻辑一致，但条件已 alias-resolved）
    for (auto &cond : query->conds) {
        TabMeta &lhs_tab = sm_manager_->db_.get_table(cond.lhs_col.tab_name);
        auto lhs_col = lhs_tab.get_col(cond.lhs_col.col_name);
        ColType lhs_type = lhs_col->type;
        ColType rhs_type;
        if (cond.is_rhs_val) {
            if (lhs_type == TYPE_FLOAT && cond.rhs_val.type == TYPE_INT) {
                cond.rhs_val.set_float((float)cond.rhs_val.int_val);
            } else if (lhs_type == TYPE_INT && cond.rhs_val.type == TYPE_FLOAT) {
                cond.rhs_val.set_int((int)cond.rhs_val.float_val);
            }
            cond.rhs_val.init_raw(lhs_col->len);
            rhs_type = cond.rhs_val.type;
        } else {
            TabMeta &rhs_tab = sm_manager_->db_.get_table(cond.rhs_col.tab_name);
            auto rhs_col = rhs_tab.get_col(cond.rhs_col.col_name);
            rhs_type = rhs_col->type;
        }
        if (lhs_type != rhs_type) {
            throw IncompatibleTypeError(coltype2str(lhs_type), coltype2str(rhs_type));
        }
    }
}


TabCol Analyze::check_column(const std::vector<ColMeta> &all_cols, TabCol target) {
    if (target.tab_name.empty()) {
        // Table name not specified, infer table name from column name
        std::string tab_name;
        for (auto &col : all_cols) {
            if (col.name == target.col_name) {
                if (!tab_name.empty()) {
                    throw AmbiguousColumnError(target.col_name);
                }
                tab_name = col.tab_name;
            }
        }
        if (tab_name.empty()) {
            throw ColumnNotFoundError(target.col_name);
        }
        target.tab_name = tab_name;
    } else {
        /** TODO: Make sure target column exists */
        
    }
    return target;
}

void Analyze::get_all_cols(const std::vector<std::string> &tab_names, std::vector<ColMeta> &all_cols) {
    for (auto &sel_tab_name : tab_names) {
        // 这里db_不能写成get_db(), 注意要传指针
        const auto &sel_tab_cols = sm_manager_->db_.get_table(sel_tab_name).cols;
        all_cols.insert(all_cols.end(), sel_tab_cols.begin(), sel_tab_cols.end());
    }
}

void Analyze::get_clause(const std::vector<std::shared_ptr<ast::BinaryExpr>> &sv_conds, std::vector<Condition> &conds) {
    conds.clear();
    for (auto &expr : sv_conds) {
        Condition cond;
        cond.lhs_col = {.tab_name = expr->lhs->tab_name, .col_name = expr->lhs->col_name};
        cond.op = convert_sv_comp_op(expr->op);
        if (auto rhs_val = std::dynamic_pointer_cast<ast::Value>(expr->rhs)) {
            cond.is_rhs_val = true;
            cond.rhs_val = convert_sv_value(rhs_val);
        } else if (auto rhs_col = std::dynamic_pointer_cast<ast::Col>(expr->rhs)) {
            cond.is_rhs_val = false;
            cond.rhs_col = {.tab_name = rhs_col->tab_name, .col_name = rhs_col->col_name};
        }
        conds.push_back(cond);
    }
}

void Analyze::check_clause(const std::vector<std::string> &tab_names, std::vector<Condition> &conds) {
    // auto all_cols = get_all_cols(tab_names);
    std::vector<ColMeta> all_cols;
    get_all_cols(tab_names, all_cols);
    // Get raw values in where clause
    for (auto &cond : conds) {
        // Infer table name from column name
        cond.lhs_col = check_column(all_cols, cond.lhs_col);
        if (!cond.is_rhs_val) {
            cond.rhs_col = check_column(all_cols, cond.rhs_col);
        }
        TabMeta &lhs_tab = sm_manager_->db_.get_table(cond.lhs_col.tab_name);
        auto lhs_col = lhs_tab.get_col(cond.lhs_col.col_name);
        ColType lhs_type = lhs_col->type;
        ColType rhs_type;
        if (cond.is_rhs_val) {
            // 类型转换：允许INT/FLOAT之间的比较
            if (lhs_type == TYPE_FLOAT && cond.rhs_val.type == TYPE_INT) {
                cond.rhs_val.set_float((float)cond.rhs_val.int_val);
            } else if (lhs_type == TYPE_INT && cond.rhs_val.type == TYPE_FLOAT) {
                cond.rhs_val.set_int((int)cond.rhs_val.float_val);
            }
            cond.rhs_val.init_raw(lhs_col->len);
            rhs_type = cond.rhs_val.type;
        } else {
            TabMeta &rhs_tab = sm_manager_->db_.get_table(cond.rhs_col.tab_name);
            auto rhs_col = rhs_tab.get_col(cond.rhs_col.col_name);
            rhs_type = rhs_col->type;
        }
        if (lhs_type != rhs_type) {
            throw IncompatibleTypeError(coltype2str(lhs_type), coltype2str(rhs_type));
        }
    }
}


Value Analyze::convert_sv_value(const std::shared_ptr<ast::Value> &sv_val) {
    Value val;
    if (auto int_lit = std::dynamic_pointer_cast<ast::IntLit>(sv_val)) {
        val.set_int(int_lit->val);
    } else if (auto float_lit = std::dynamic_pointer_cast<ast::FloatLit>(sv_val)) {
        val.set_float(float_lit->val);
    } else if (auto str_lit = std::dynamic_pointer_cast<ast::StringLit>(sv_val)) {
        val.set_str(str_lit->val);
    } else {
        throw InternalError("Unexpected sv value type");
    }
    return val;
}

CompOp Analyze::convert_sv_comp_op(ast::SvCompOp op) {
    std::map<ast::SvCompOp, CompOp> m = {
        {ast::SV_OP_EQ, OP_EQ}, {ast::SV_OP_NE, OP_NE}, {ast::SV_OP_LT, OP_LT},
        {ast::SV_OP_GT, OP_GT}, {ast::SV_OP_LE, OP_LE}, {ast::SV_OP_GE, OP_GE},
    };
    return m.at(op);
}
