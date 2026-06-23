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

#include <algorithm>

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
    } else if (auto x = std::dynamic_pointer_cast<ast::UnionStmt>(parse)) {
        analyze_union(x, query);
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
            // 查找列元数据，进行类型转换和init_raw
            auto col_meta = tab.get_col(sv_set_clause->col_name);
            if (col_meta == tab.cols.end()) {
                throw ColumnNotFoundError(sv_set_clause->col_name);
            }
            if (auto rhs_val = std::dynamic_pointer_cast<ast::Value>(sv_set_clause->rhs)) {
                set_clause.rhs = convert_sv_value(rhs_val);
                // INT-FLOAT转换
                if (col_meta->type == TYPE_FLOAT && set_clause.rhs.type == TYPE_INT) {
                    set_clause.rhs.set_float((float)set_clause.rhs.int_val);
                } else if (col_meta->type == TYPE_INT && set_clause.rhs.type == TYPE_FLOAT) {
                    set_clause.rhs.set_int((int)set_clause.rhs.float_val);
                }
                set_clause.rhs.init_raw(col_meta->len);
            } else if (auto rhs_expr = std::dynamic_pointer_cast<ast::ArithmeticExpr>(sv_set_clause->rhs)) {
                auto rhs_col = tab.get_col(rhs_expr->lhs->col_name);
                if (rhs_col == tab.cols.end()) {
                    throw ColumnNotFoundError(rhs_expr->lhs->col_name);
                }
                if (!rhs_expr->lhs->tab_name.empty() && rhs_expr->lhs->tab_name != x->tab_name) {
                    throw ColumnNotFoundError(rhs_expr->lhs->tab_name + "." + rhs_expr->lhs->col_name);
                }
                if ((col_meta->type != TYPE_INT && col_meta->type != TYPE_FLOAT) ||
                    (rhs_col->type != TYPE_INT && rhs_col->type != TYPE_FLOAT)) {
                    throw IncompatibleTypeError(coltype2str(col_meta->type), coltype2str(rhs_col->type));
                }
                set_clause.is_rhs_expr = true;
                set_clause.rhs_col = {.tab_name = x->tab_name, .col_name = rhs_expr->lhs->col_name};
                set_clause.op = rhs_expr->op;
                set_clause.rhs = convert_sv_value(rhs_expr->rhs);
                if (set_clause.rhs.type != TYPE_INT && set_clause.rhs.type != TYPE_FLOAT) {
                    throw IncompatibleTypeError(coltype2str(rhs_col->type), coltype2str(set_clause.rhs.type));
                }
            } else {
                throw RMDBError("failure");
            }
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

std::shared_ptr<Query> Analyze::analyze_query_expr(const std::shared_ptr<ast::TreeNode> &root) {
    return do_analyze(root);
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

static std::vector<ColMeta> make_output_cols(const std::vector<TabCol> &cols,
                                             const std::vector<ColMeta> &source_cols) {
    std::vector<ColMeta> out;
    int offset = 0;
    for (auto &tc : cols) {
        auto pos = std::find_if(source_cols.begin(), source_cols.end(), [&](const ColMeta &col) {
            return col.tab_name == tc.tab_name && col.name == tc.col_name;
        });
        if (pos == source_cols.end()) {
            throw ColumnNotFoundError(tc.tab_name + "." + tc.col_name);
        }
        ColMeta meta = *pos;
        meta.name = tc.col_name;
        meta.offset = offset;
        offset += meta.len;
        out.push_back(meta);
    }
    return out;
}

static ColType union_common_type(const ColMeta &lhs, const ColMeta &rhs, int &len) {
    if (lhs.type == rhs.type) {
        len = lhs.type == TYPE_STRING ? std::max(lhs.len, rhs.len) : lhs.len;
        return lhs.type;
    }
    bool numeric = (lhs.type == TYPE_INT && rhs.type == TYPE_FLOAT) ||
                   (lhs.type == TYPE_FLOAT && rhs.type == TYPE_INT);
    if (numeric) {
        len = sizeof(float);
        return TYPE_FLOAT;
    }
    throw RMDBError("failure");
}

void Analyze::analyze_union(const std::shared_ptr<ast::UnionStmt> &x, std::shared_ptr<Query> query) {
    if (x->branches.size() < 2) {
        throw RMDBError("failure");
    }
    for (auto &branch : x->branches) {
        auto child = analyze_query_expr(branch);
        if (child->output_cols.empty()) {
            throw RMDBError("failure");
        }
        query->union_children.push_back(child);
    }

    size_t col_num = query->union_children[0]->output_cols.size();
    for (auto &child : query->union_children) {
        if (child->output_cols.size() != col_num) {
            throw RMDBError("failure");
        }
    }

    query->cols.clear();
    query->output_cols.clear();
    int offset = 0;
    for (size_t i = 0; i < col_num; i++) {
        ColMeta common = query->union_children[0]->output_cols[i];
        int len = common.len;
        for (size_t j = 1; j < query->union_children.size(); j++) {
            common.type = union_common_type(common, query->union_children[j]->output_cols[i], len);
            common.len = len;
        }
        common.tab_name = "__union";
        common.name = query->union_children[0]->output_cols[i].name;
        common.offset = offset;
        common.index = false;
        offset += common.len;
        query->output_cols.push_back(common);
        query->cols.push_back({common.tab_name, common.name, ""});
    }
}

// 题目四：分析 SELECT 主体，支持表别名（FROM customers c）。
// 关键点：匹配/执行一律用真实表名（写入 TabCol.tab_name），别名仅写入 tab_alias 供 EXPLAIN 显示。
void Analyze::analyze_select(const std::shared_ptr<ast::SelectStmt> &x, std::shared_ptr<Query> query) {
    query->tables.clear();
    query->sources.clear();

    std::vector<std::shared_ptr<ast::TableRef>> refs = x->from_refs;
    if (refs.empty()) {
        for (size_t i = 0; i < x->tabs.size(); i++) {
            std::string alias = i < x->tab_alias.size() ? x->tab_alias[i] : std::string();
            refs.push_back(std::make_shared<ast::TableRef>(x->tabs[i], alias));
        }
    }

    // 构建 “SQL可见名 -> 执行内部名” 与 “执行内部名 -> 显示别名” 两张映射。
    std::map<std::string, std::string> name2real;
    std::map<std::string, std::string> real2alias;
    std::vector<ColMeta> all_cols;
    for (auto &ref : refs) {
        QuerySource source;
        if (ref->is_derived) {
            if (ref->alias.empty()) {
                throw RMDBError("failure");
            }
            source.is_derived = true;
            source.name = ref->alias;
            source.alias = ref->alias;
            source.derived_query = analyze_query_expr(ref->derived_query);
            int offset = 0;
            for (auto col : source.derived_query->output_cols) {
                col.tab_name = source.name;
                col.offset = offset;
                col.index = false;
                offset += col.len;
                source.cols.push_back(col);
            }
            name2real[source.name] = source.name;
        } else {
            source.name = ref->tab_name;
            source.alias = ref->alias;
            TabMeta &tab = sm_manager_->db_.get_table(source.name);
            source.cols = tab.cols;
            name2real[source.name] = source.name;
            if (!source.alias.empty()) {
                name2real[source.alias] = source.name;
                real2alias[source.name] = source.alias;
            }
        }
        query->tables.push_back(source.name);
        all_cols.insert(all_cols.end(), source.cols.begin(), source.cols.end());
        query->sources.push_back(std::move(source));
    }

    // 解析一个列引用：把（可能是别名的）表限定符解析为真实表名，并填好显示用别名
    auto resolve_col = [&](const std::string &tok, const std::string &col_name) -> TabCol {
        TabCol tc;
        tc.col_name = col_name;
        std::string real;
        if (!tok.empty()) {
            auto it = name2real.find(tok);
            if (it == name2real.end()) {
                throw RMDBError("failure");
            }
            real = it->second;
        } else {
            // 未指定表名，根据列名在所有表中推断
            for (auto &col : all_cols) {
                if (col.name == col_name) {
                    if (!real.empty() && real != col.tab_name) {
                        throw RMDBError("failure");
                    }
                    real = col.tab_name;
                }
            }
            if (real.empty()) {
                throw RMDBError("failure");
            }
        }
        tc.tab_name = real;
        bool exists = false;
        for (auto &col : all_cols) {
            if (col.tab_name == real && col.name == col_name) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            throw RMDBError("failure");
        }
        auto ait = real2alias.find(real);
        tc.tab_alias = (ait != real2alias.end()) ? ait->second : std::string();
        return tc;
    };

    auto get_col_meta = [&](const TabCol &tc) -> ColMeta {
        auto pos = std::find_if(all_cols.begin(), all_cols.end(), [&](const ColMeta &col) {
            return col.tab_name == tc.tab_name && col.name == tc.col_name;
        });
        if (pos == all_cols.end()) {
            throw RMDBError("failure");
        }
        return *pos;
    };

    auto ast_agg_type = [](ast::AggFuncType type) -> AggFuncType {
        switch (type) {
            case ast::Agg_COUNT: return AGG_COUNT;
            case ast::Agg_MAX: return AGG_MAX;
            case ast::Agg_MIN: return AGG_MIN;
            case ast::Agg_SUM: return AGG_SUM;
            case ast::Agg_AVG: return AGG_AVG;
        }
        return AGG_COUNT;
    };

    auto add_agg_call = [&](const std::shared_ptr<ast::AggExpr> &agg) -> int {
        AggCall call;
        call.type = ast_agg_type(agg->func);
        call.is_star = agg->is_star;
        if (!call.is_star) {
            call.col = resolve_col(agg->col->tab_name, agg->col->col_name);
            auto meta = get_col_meta(call.col);
            call.arg_type = meta.type;
            call.result_len = meta.len;
        }

        if (call.type == AGG_COUNT) {
            call.result_type = TYPE_INT;
            call.result_len = sizeof(int);
        } else {
            if (call.is_star) {
                throw RMDBError("failure");
            }
            if (call.arg_type == TYPE_STRING && call.type != AGG_MAX && call.type != AGG_MIN) {
                throw RMDBError("failure");
            }
            if (call.arg_type != TYPE_INT && call.arg_type != TYPE_FLOAT && call.arg_type != TYPE_STRING) {
                throw RMDBError("failure");
            }
            if (call.type == AGG_AVG) {
                call.result_type = TYPE_FLOAT;
                call.result_len = sizeof(float);
            } else {
                call.result_type = call.arg_type;
                call.result_len = (call.arg_type == TYPE_INT) ? sizeof(int) :
                                  (call.arg_type == TYPE_FLOAT ? sizeof(float) : call.result_len);
            }
        }

        std::string key = call.key();
        for (size_t i = 0; i < query->agg_calls.size(); i++) {
            if (query->agg_calls[i].key() == key) {
                return (int)i;
            }
        }
        query->agg_calls.push_back(call);
        return (int)query->agg_calls.size() - 1;
    };

    auto default_agg_name = [](const AggCall &call) {
        return agg_func_name(call.type) + "(" + (call.is_star ? "*" : call.col.col_name) + ")";
    };

    auto group_contains = [&](const TabCol &tc) {
        for (auto &g : query->group_cols) {
            if (g.tab_name == tc.tab_name && g.col_name == tc.col_name) return true;
        }
        return false;
    };

    // 处理 GROUP BY
    for (auto &g : x->group_cols) {
        query->group_cols.push_back(resolve_col(g->tab_name, g->col_name));
    }

    // 处理投影列
    if (x->items.empty()) {
        if (!query->group_cols.empty() || !x->having_conds.empty()) {
            throw RMDBError("failure");
        }
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
        for (auto &item : x->items) {
            SelectTerm term;
            if (auto sv_col = std::dynamic_pointer_cast<ast::Col>(item->expr)) {
                term.is_agg = false;
                term.col = resolve_col(sv_col->tab_name, sv_col->col_name);
                auto meta = get_col_meta(term.col);
                term.type = meta.type;
                term.len = meta.len;
                term.output_name = item->alias.empty() ? term.col.col_name : item->alias;
            } else if (auto sv_agg = std::dynamic_pointer_cast<ast::AggExpr>(item->expr)) {
                term.is_agg = true;
                term.agg_idx = add_agg_call(sv_agg);
                auto &call = query->agg_calls[term.agg_idx];
                term.type = call.result_type;
                term.len = call.result_len;
                term.output_name = item->alias.empty() ? default_agg_name(call) : item->alias;
                query->has_aggregate = true;
            } else {
                throw RMDBError("failure");
            }
            query->select_terms.push_back(term);

            TabCol out_col;
            out_col.tab_name = query->has_aggregate || !query->group_cols.empty() ? "__agg" : term.col.tab_name;
            out_col.col_name = term.output_name;
            out_col.tab_alias = term.col.tab_alias;
            query->cols.push_back(out_col);
        }
    }

    query->has_aggregate = query->has_aggregate || !query->group_cols.empty() || !x->having_conds.empty();

    if (query->has_aggregate) {
        for (auto &term : query->select_terms) {
            if (!term.is_agg && !group_contains(term.col)) {
                throw RMDBError("failure");
            }
        }
        for (auto &out_col : query->cols) {
            out_col.tab_name = "__agg";
        }
    }

    // 处理 where / JOIN ON 条件（已在语法层合并到 x->conds）
    query->conds.clear();
    for (auto &expr : x->conds) {
        if (expr->lhs->col_name == "__agg_in_where__") {
            throw RMDBError("failure");
        }
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
        auto lhs_col = get_col_meta(cond.lhs_col);
        ColType lhs_type = lhs_col.type;
        ColType rhs_type;
        if (cond.is_rhs_val) {
            if (lhs_type == TYPE_FLOAT && cond.rhs_val.type == TYPE_INT) {
                cond.rhs_val.set_float((float)cond.rhs_val.int_val);
            } else if (lhs_type == TYPE_INT && cond.rhs_val.type == TYPE_FLOAT) {
                cond.rhs_val.set_int((int)cond.rhs_val.float_val);
            }
            cond.rhs_val.init_raw(lhs_col.len);
            rhs_type = cond.rhs_val.type;
        } else {
            auto rhs_col = get_col_meta(cond.rhs_col);
            rhs_type = rhs_col.type;
        }
        if (lhs_type != rhs_type) {
            throw IncompatibleTypeError(coltype2str(lhs_type), coltype2str(rhs_type));
        }
    }

    auto convert_having_term = [&](const std::shared_ptr<ast::Expr> &expr) -> AggTerm {
        AggTerm term;
        if (auto sv_agg = std::dynamic_pointer_cast<ast::AggExpr>(expr)) {
            term.kind = AGG_TERM_AGG;
            term.agg_idx = add_agg_call(sv_agg);
            auto &call = query->agg_calls[term.agg_idx];
            term.type = call.result_type;
            term.len = call.result_len;
            query->has_aggregate = true;
        } else if (auto sv_col = std::dynamic_pointer_cast<ast::Col>(expr)) {
            term.kind = AGG_TERM_COL;
            term.col = resolve_col(sv_col->tab_name, sv_col->col_name);
            if (!group_contains(term.col)) {
                throw RMDBError("failure");
            }
            auto meta = get_col_meta(term.col);
            term.type = meta.type;
            term.len = meta.len;
        } else if (auto sv_val = std::dynamic_pointer_cast<ast::Value>(expr)) {
            term.kind = AGG_TERM_VALUE;
            term.val = convert_sv_value(sv_val);
            term.type = term.val.type;
            term.len = (term.type == TYPE_STRING) ? (int)term.val.str_val.size() : (int)sizeof(int);
            if (term.type == TYPE_FLOAT) term.len = sizeof(float);
        } else {
            throw RMDBError("failure");
        }
        return term;
    };

    // 处理 HAVING
    for (auto &sv_having : x->having_conds) {
        AggHavingCond cond;
        cond.lhs = convert_having_term(sv_having->lhs);
        cond.op = convert_sv_comp_op(sv_having->op);
        cond.rhs = convert_having_term(sv_having->rhs);
        bool numeric = (cond.lhs.type == TYPE_INT || cond.lhs.type == TYPE_FLOAT) &&
                       (cond.rhs.type == TYPE_INT || cond.rhs.type == TYPE_FLOAT);
        if (!numeric && cond.lhs.type != cond.rhs.type) {
            throw RMDBError("failure");
        }
        query->having_conds.push_back(cond);
    }

    if (query->has_aggregate) {
        for (auto &term : query->select_terms) {
            if (!term.is_agg && !group_contains(term.col)) {
                throw RMDBError("failure");
            }
        }
        for (auto &out_col : query->cols) {
            out_col.tab_name = "__agg";
        }
    }

    // 处理 ORDER BY / LIMIT
    if (x->has_sort) {
        for (size_t i = 0; i < x->order->cols.size(); i++) {
            OrderByTerm order;
            auto &sv_col = x->order->cols[i];
            if (query->has_aggregate) {
                bool found = false;
                for (auto &out : query->cols) {
                    if (out.col_name == sv_col->col_name) {
                        order.col = out;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    TabCol raw = resolve_col(sv_col->tab_name, sv_col->col_name);
                    if (!group_contains(raw)) throw RMDBError("failure");
                    order.col = {.tab_name = "__agg", .col_name = raw.col_name};
                }
            } else {
                order.col = resolve_col(sv_col->tab_name, sv_col->col_name);
            }
            order.is_desc = i < x->order->orderby_dirs.size() && x->order->orderby_dirs[i] == ast::OrderBy_DESC;
            query->order_bys.push_back(order);
        }
    }
    query->limit = x->limit;
    query->output_cols.clear();
    int out_offset = 0;
    if (x->items.empty()) {
        for (auto &tc : query->cols) {
            auto meta = get_col_meta(tc);
            meta.name = tc.col_name;
            meta.offset = out_offset;
            out_offset += meta.len;
            query->output_cols.push_back(meta);
        }
    } else {
        for (auto &term : query->select_terms) {
            ColMeta meta;
            if (term.is_agg) {
                meta.tab_name = query->has_aggregate ? "__agg" : "";
                meta.name = term.output_name;
                meta.type = term.type;
                meta.len = term.len;
                meta.offset = out_offset;
                meta.index = false;
            } else {
                meta = get_col_meta(term.col);
                meta.name = term.output_name;
                meta.offset = out_offset;
            }
            out_offset += meta.len;
            query->output_cols.push_back(meta);
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
