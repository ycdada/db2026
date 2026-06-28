/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "planner.h"

#include <memory>
#include <unordered_map>

#include "execution/executor_delete.h"
#include "execution/executor_index_scan.h"
#include "execution/executor_insert.h"
#include "execution/executor_nestedloop_join.h"
#include "execution/executor_projection.h"
#include "execution/executor_seq_scan.h"
#include "execution/executor_update.h"
#include "index/ix.h"
#include "record/rm_scan.h"
#include "record_printer.h"

namespace {

CompOp swap_comp_op(CompOp op) {
    switch (op) {
        case OP_EQ: return OP_EQ;
        case OP_NE: return OP_NE;
        case OP_LT: return OP_GT;
        case OP_GT: return OP_LT;
        case OP_LE: return OP_GE;
        case OP_GE: return OP_LE;
    }
    return op;
}

}

bool Planner::get_index_cols(const std::string &tab_name, const std::vector<Condition> &curr_conds,
                             std::vector<std::string> &index_col_names) {
    index_col_names.clear();
    TabMeta& tab = sm_manager_->db_.get_table(tab_name);
    std::unordered_map<std::string, std::vector<const Condition *>> cond_by_col;
    cond_by_col.reserve(curr_conds.size());
    for (auto &cond : curr_conds) {
        if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name) {
            cond_by_col[cond.lhs_col.col_name].push_back(&cond);
        }
    }
    size_t best_match = 0;
    std::vector<std::string> best_cols;
    for (auto &index : tab.indexes) {
        size_t matched = 0;
        for (auto &col : index.cols) {
            auto cond_it = cond_by_col.find(col.name);
            if (cond_it == cond_by_col.end()) break;
            bool found = false;
            for (auto *cond : cond_it->second) {
                if (cond->op != OP_NE) {
                    found = true;
                    break;
                }
            }
            if (!found) break;
            matched++;
            bool has_range = false;
            for (auto *cond : cond_it->second) {
                if (cond->op != OP_EQ && cond->op != OP_NE) {
                    has_range = true;
                    break;
                }
            }
            if (has_range) break;
        }
        if (matched > best_match) {
            best_match = matched;
            best_cols.clear();
            for (auto &col : index.cols) {
                best_cols.push_back(col.name);
            }
        }
    }
    if (best_match > 0) {
        index_col_names = best_cols;
        return true;
    }
    return false;
}

/**
 * @brief 表算子条件谓词生成
 *
 * @param conds 条件
 * @param tab_names 表名
 * @return std::vector<Condition>
 */
std::vector<Condition> pop_conds(std::vector<Condition> &conds, std::string tab_names) {
    // auto has_tab = [&](const std::string &tab_name) {
    //     return std::find(tab_names.begin(), tab_names.end(), tab_name) != tab_names.end();
    // };
    std::vector<Condition> solved_conds;
    auto it = conds.begin();
    while (it != conds.end()) {
        if ((tab_names.compare(it->lhs_col.tab_name) == 0 && it->is_rhs_val) || (it->lhs_col.tab_name.compare(it->rhs_col.tab_name) == 0)) {
            solved_conds.emplace_back(std::move(*it));
            it = conds.erase(it);
        } else {
            it++;
        }
    }
    return solved_conds;
}

static bool plan_has_source(const std::shared_ptr<Plan> &plan, const std::string &source)
{
    if(auto x = std::dynamic_pointer_cast<ScanPlan>(plan)) {
        return x->tab_name_ == source;
    } else if (auto x = std::dynamic_pointer_cast<RenamePlan>(plan)) {
        return !x->cols_.empty() && x->cols_[0].tab_name == source;
    } else if (auto x = std::dynamic_pointer_cast<FilterPlan>(plan)) {
        return plan_has_source(x->subplan_, source);
    } else if (auto x = std::dynamic_pointer_cast<ProjectionPlan>(plan)) {
        return plan_has_source(x->subplan_, source);
    } else if (auto x = std::dynamic_pointer_cast<JoinPlan>(plan)) {
        return plan_has_source(x->left_, source) || plan_has_source(x->right_, source);
    }
    return false;
}

int push_conds(Condition *cond, std::shared_ptr<Plan> plan)
{
    if(auto x = std::dynamic_pointer_cast<ScanPlan>(plan))
    {
        if(x->tab_name_.compare(cond->lhs_col.tab_name) == 0) {
            return 1;
        } else if(x->tab_name_.compare(cond->rhs_col.tab_name) == 0){
            return 2;
        } else {
            return 0;
        }
    }
    else if(auto x = std::dynamic_pointer_cast<JoinPlan>(plan))
    {
        int left_res = push_conds(cond, x->left_);
        // 条件已经下推到左子节点
        if(left_res == 3){
            return 3;
        }
        int right_res = push_conds(cond, x->right_);
        // 条件已经下推到右子节点
        if(right_res == 3){
            return 3;
        }
        // 左子节点或右子节点有一个没有匹配到条件的列
        if(left_res == 0 || right_res == 0) {
            return left_res + right_res;
        }
        // 左子节点匹配到条件的右边
        if(left_res == 2) {
            // 需要将左右两边的条件变换位置
            std::swap(cond->lhs_col, cond->rhs_col);
            cond->op = swap_comp_op(cond->op);
        }
        x->conds_.emplace_back(std::move(*cond));
        return 3;
    }
    else if(auto x = std::dynamic_pointer_cast<FilterPlan>(plan))
    {
        // 题目四：透传到子节点
        return push_conds(cond, x->subplan_);
    }
    else if(auto x = std::dynamic_pointer_cast<RenamePlan>(plan))
    {
        return plan_has_source(plan, cond->lhs_col.tab_name) ? 1 :
               (!cond->is_rhs_val && plan_has_source(plan, cond->rhs_col.tab_name) ? 2 : 0);
    }
    else if(auto x = std::dynamic_pointer_cast<ProjectionPlan>(plan))
    {
        // 题目四：透传到子节点
        return push_conds(cond, x->subplan_);
    }
    return false;
}

std::shared_ptr<Plan> pop_scan(int *scantbl, std::string table, std::vector<std::string> &joined_tables, 
                std::vector<std::shared_ptr<Plan>> plans)
{
    for (size_t i = 0; i < plans.size(); i++) {
        if(plan_has_source(plans[i], table))
        {
            scantbl[i] = 1;
            joined_tables.emplace_back(table);
            return plans[i];
        }
    }
    return nullptr;
}


std::shared_ptr<Query> Planner::logical_optimization(std::shared_ptr<Query> query, Context *context)
{
    
    //TODO 实现逻辑优化规则

    return query;
}

std::shared_ptr<Plan> Planner::make_query_plan(std::shared_ptr<Query> query, Context *context) {
    if (!query->union_children.empty()) {
        std::vector<std::shared_ptr<Plan>> children;
        for (auto &child : query->union_children) {
            children.push_back(generate_select_plan(child, context));
        }
        return std::make_shared<UnionPlan>(std::move(children), query->output_cols);
    }
    return generate_select_plan(query, context);
}

std::shared_ptr<Plan> Planner::build_source_plan(const QuerySource &source, Context *context) {
    if (!source.is_derived) {
        return nullptr;
    }
    auto plan = make_query_plan(source.derived_query, context);
    return std::make_shared<RenamePlan>(std::move(plan), source.cols);
}

std::shared_ptr<Plan> Planner::physical_optimization(std::shared_ptr<Query> query, Context *context)
{
    std::shared_ptr<Plan> plan;
    if (!query->union_children.empty()) {
        std::vector<std::shared_ptr<Plan>> children;
        for (auto &child : query->union_children) {
            children.push_back(generate_select_plan(child, context));
        }
        plan = std::make_shared<UnionPlan>(std::move(children), query->output_cols);
    } else {
        plan = make_one_rel(query, context);
    }
    
    // 其他物理优化
    if (query->has_aggregate) {
        plan = std::make_shared<AggregatePlan>(std::move(plan), query->select_terms, query->agg_calls,
                                               query->group_cols, query->having_conds);
    }

    // 处理orderby
    plan = generate_sort_plan(query, std::move(plan));

    if (query->limit >= 0) {
        plan = std::make_shared<LimitPlan>(std::move(plan), query->limit);
    }

    return plan;
}

static bool table_in_list(const std::vector<std::string> &tabs, const std::string &tab)
{
    return std::find(tabs.begin(), tabs.end(), tab) != tabs.end();
}

static bool is_single_table_cond(const Condition &cond, const std::string &tab)
{
    if (cond.lhs_col.tab_name != tab) return false;
    return cond.is_rhs_val || cond.rhs_col.tab_name == tab;
}

static std::vector<Condition> take_single_table_conds(std::vector<Condition> &conds, const std::string &tab)
{
    std::vector<Condition> out;
    auto it = conds.begin();
    while (it != conds.end()) {
        if (is_single_table_cond(*it, tab)) {
            out.emplace_back(std::move(*it));
            it = conds.erase(it);
        } else {
            ++it;
        }
    }
    return out;
}

static std::vector<Condition> take_join_conds(std::vector<Condition> &conds,
                                               const std::vector<std::string> &joined,
                                               const std::string &right_tab)
{
    std::vector<Condition> out;
    auto it = conds.begin();
    while (it != conds.end()) {
        if (!it->is_rhs_val) {
            bool lhs_joined = table_in_list(joined, it->lhs_col.tab_name);
            bool rhs_joined = table_in_list(joined, it->rhs_col.tab_name);
            bool lhs_right = it->lhs_col.tab_name == right_tab;
            bool rhs_right = it->rhs_col.tab_name == right_tab;
            if ((lhs_joined && rhs_right) || (rhs_joined && lhs_right)) {
                out.push_back(*it);
                it = conds.erase(it);
                continue;
            }
        }
        ++it;
    }
    return out;
}

static bool choose_inlj_key(SmManager *sm_manager, const std::string &right_tab,
                            const std::vector<std::string> &joined,
                            const std::vector<Condition> &join_conds,
                            TabCol &outer_col, std::string &inner_col)
{
    TabMeta &tab = sm_manager->db_.get_table(right_tab);
    for (auto &cond : join_conds) {
        if (cond.op != OP_EQ || cond.is_rhs_val) continue;
        if (cond.lhs_col.tab_name == right_tab && table_in_list(joined, cond.rhs_col.tab_name) &&
            tab.is_index({cond.lhs_col.col_name})) {
            outer_col = cond.rhs_col;
            inner_col = cond.lhs_col.col_name;
            return true;
        }
        if (cond.rhs_col.tab_name == right_tab && table_in_list(joined, cond.lhs_col.tab_name) &&
            tab.is_index({cond.rhs_col.col_name})) {
            outer_col = cond.lhs_col;
            inner_col = cond.rhs_col.col_name;
            return true;
        }
    }
    return false;
}

std::shared_ptr<Plan> Planner::build_source_scan_plan(const QuerySource *source, const std::string &table,
                                                       std::vector<Condition> conds, Context *context,
                                                       bool is_join_inner, const TabCol &outer_col,
                                                       const std::string &inner_col)
{
    if (source != nullptr && source->is_derived) {
        auto plan = build_source_plan(*source, context);
        if (!conds.empty()) {
            plan = std::make_shared<FilterPlan>(std::move(plan), std::move(conds));
        }
        return plan;
    }

    std::vector<std::string> index_col_names;
    PlanTag tag = T_SeqScan;
    if (is_join_inner) {
        index_col_names = {inner_col};
        tag = T_IndexScan;
    } else if (get_index_cols(table, conds, index_col_names)) {
        tag = T_IndexScan;
    } else {
        index_col_names.clear();
    }

    auto scan = std::make_shared<ScanPlan>(tag, sm_manager_, table, std::move(conds), index_col_names);
    if (is_join_inner) {
        scan->is_join_inner_ = true;
        scan->join_outer_col_ = outer_col;
        scan->join_inner_col_ = inner_col;
    }
    return scan;
}



std::shared_ptr<Plan> Planner::make_one_rel(std::shared_ptr<Query> query, Context *context)
{
    const std::vector<std::string> &tables = query->tables;
    if (tables.empty()) return nullptr;

    std::vector<Condition> conds = std::move(query->conds);
    std::vector<std::vector<Condition>> single_conds(tables.size());
    for (size_t i = 0; i < tables.size(); i++) {
        single_conds[i] = take_single_table_conds(conds, tables[i]);
    }

    auto source_at = [&](size_t i) -> const QuerySource * {
        return i < query->sources.size() ? &query->sources[i] : nullptr;
    };

    std::shared_ptr<Plan> root = build_source_scan_plan(source_at(0), tables[0], single_conds[0], context, false, {}, "");
    std::vector<std::string> joined{tables[0]};

    for (size_t i = 1; i < tables.size(); i++) {
        std::vector<Condition> join_conds = take_join_conds(conds, joined, tables[i]);
        TabCol outer_col;
        std::string inner_col;
        bool use_inlj = source_at(i) != nullptr && !source_at(i)->is_derived &&
                        choose_inlj_key(sm_manager_, tables[i], joined, join_conds, outer_col, inner_col);
        auto right = build_source_scan_plan(source_at(i), tables[i], single_conds[i], context,
                                            use_inlj, outer_col, inner_col);
        root = std::make_shared<JoinPlan>(T_NestLoop, std::move(root), std::move(right), join_conds);
        joined.push_back(tables[i]);
    }

    for (auto &cond : conds) {
        push_conds(&cond, root);
    }

    return root;

}


std::shared_ptr<Plan> Planner::generate_sort_plan(std::shared_ptr<Query> query, std::shared_ptr<Plan> plan)
{
    if(query->order_bys.empty()) {
        return plan;
    }
    return std::make_shared<SortPlan>(T_Sort, std::move(plan), query->order_bys);
}


/**
 * @brief select plan 生成
 *
 * @param sel_cols select plan 选取的列
 * @param tab_names select plan 目标的表
 * @param conds select plan 选取条件
 */
std::shared_ptr<Plan> Planner::generate_select_plan(std::shared_ptr<Query> query, Context *context) {
    //逻辑优化
    query = logical_optimization(std::move(query), context);

    //物理优化
    auto sel_cols = query->cols;
    std::shared_ptr<Plan> plannerRoot = physical_optimization(query, context);
    plannerRoot = std::make_shared<ProjectionPlan>(T_Projection, std::move(plannerRoot),
                                                        std::move(sel_cols));

    return plannerRoot;
}

// ============== 题目四：EXPLAIN ANALYZE 计划构建 ==============

// 统计表的基数（行数），用于连接顺序优化：扫描一遍 RmFileHandle 计数
size_t Planner::get_table_cardinality(const std::string &tab_name) {
    auto fh = sm_manager_->fhs_.at(tab_name).get();
    size_t cnt = 0;
    RmScan scan(fh);
    while (!scan.is_end()) {
        cnt++;
        scan.next();
    }
    return cnt;
}

// 取出属于单表的谓词（左列属于该表且右值为常量，或左右列同属一表同别名）
// 关键：对于 col1 = col2 形式，若两列分属不同别名（自连接），视为跨表连接条件不予提取。
static std::vector<Condition> explain_pop_single_conds(std::vector<Condition> &conds, const std::string &tab) {
    std::vector<Condition> solved;
    auto it = conds.begin();
    while (it != conds.end()) {
        if (it->lhs_col.tab_name == tab && it->is_rhs_val) {
            // 单表常量过滤：左列属于当前表
            solved.emplace_back(std::move(*it));
            it = conds.erase(it);
        } else if (!it->is_rhs_val && it->lhs_col.tab_name == tab && it->rhs_col.tab_name == tab) {
            // 左右列都属同一物理表：检查别名，避免把自连接条件误认为单表条件
            if (it->lhs_col.tab_alias == it->rhs_col.tab_alias) {
                solved.emplace_back(std::move(*it));
                it = conds.erase(it);
            } else {
                ++it;  // 自连接条件（别名不同），留在 conds 中作为连接条件
            }
        } else {
            ++it;
        }
    }
    return solved;
}

// 收集一棵计划子树覆盖的所有真实表名
static void explain_collect_tables(const std::shared_ptr<Plan> &plan, std::vector<std::string> &out) {
    if (auto x = std::dynamic_pointer_cast<ScanPlan>(plan)) {
        out.push_back(x->tab_name_);
    } else if (auto x = std::dynamic_pointer_cast<FilterPlan>(plan)) {
        explain_collect_tables(x->subplan_, out);
    } else if (auto x = std::dynamic_pointer_cast<ProjectionPlan>(plan)) {
        explain_collect_tables(x->subplan_, out);
    } else if (auto x = std::dynamic_pointer_cast<RenamePlan>(plan)) {
        explain_collect_tables(x->subplan_, out);
    } else if (auto x = std::dynamic_pointer_cast<JoinPlan>(plan)) {
        explain_collect_tables(x->left_, out);
        explain_collect_tables(x->right_, out);
    }
}

// 判断连接条件在此次连接中是否可用：一侧在已连接集合，另一侧恰为当前新加入的表
// 匹配参考实现 join_tables 中的严格条件：(left_in_joined && right_in_current) || (right_in_joined && left_in_current)
static bool cond_applicable(const Condition &c, const std::vector<std::string> &joined,
                            const std::string &current) {
    if (c.is_rhs_val) return false;  // 单表条件不应残留在此
    bool lhs_in = std::find(joined.begin(), joined.end(), c.lhs_col.tab_name) != joined.end();
    bool rhs_in = std::find(joined.begin(), joined.end(), c.rhs_col.tab_name) != joined.end();
    bool lhs_cur = c.lhs_col.tab_name == current;
    bool rhs_cur = c.rhs_col.tab_name == current;
    return (lhs_in && rhs_cur) || (rhs_in && lhs_cur);
}

std::shared_ptr<Plan> Planner::generate_explain_plan(std::shared_ptr<Query> query, Context *context) {
    std::vector<std::string> tables = query->tables;
    std::vector<Condition> conds = query->conds;  // 拷贝，下面会消耗
    if (tables.empty()) return nullptr;

    // 1) 谓词下推：先把每张表的单表条件全部取出，剩下的 conds 即为跨表连接条件。
    //    必须先全部取出，否则投影下推时仍会把别的表尚未处理的单表谓词列误并入投影。
    bool multi = tables.size() > 1;

    // 对 tables 去重后提取单表条件，避免自连接场景下同名表覆盖 single_conds 导致条件丢失
    std::vector<std::string> unique_tabs = tables;
    std::sort(unique_tabs.begin(), unique_tabs.end());
    unique_tabs.erase(std::unique(unique_tabs.begin(), unique_tabs.end()), unique_tabs.end());

    std::map<std::string, std::vector<Condition>> single_conds;
    for (auto &tab : unique_tabs) {
        single_conds[tab] = explain_pop_single_conds(conds, tab);
    }
    std::vector<std::string> joined;
    std::shared_ptr<Plan> root;
    for (size_t i = 0; i < tables.size(); i++) {
        const std::string &tab = tables[i];
        const QuerySource *source = i < query->sources.size() ? &query->sources[i] : nullptr;
        std::vector<Condition> remaining_conds = conds;
        std::vector<Condition> join_conds;
        if (i > 0) {
            auto it = conds.begin();
            while (it != conds.end()) {
                if (cond_applicable(*it, joined, tab)) {
                    join_conds.push_back(*it);
                    it = conds.erase(it);
                } else {
                    ++it;
                }
            }
        }

        TabCol outer_col;
        std::string inner_col;
	        bool use_inlj = source != nullptr && !source->is_derived &&
	                        choose_inlj_key(sm_manager_, tab, joined, join_conds, outer_col, inner_col);

        std::shared_ptr<Plan> leaf;
        if (source != nullptr && source->is_derived) {
            leaf = build_source_plan(*source, context);
        } else {
            std::vector<std::string> index_col_names;
            PlanTag scan_tag = use_inlj ? T_IndexScan : T_SeqScan;
            if (use_inlj) {
                index_col_names = {inner_col};
            }
            auto scan = std::make_shared<ScanPlan>(scan_tag, sm_manager_, tab, std::vector<Condition>(), index_col_names);
            if (use_inlj) {
                scan->is_join_inner_ = true;
                scan->join_outer_col_ = outer_col;
                scan->join_inner_col_ = inner_col;
            }
            leaf = scan;
        }

        auto &single = single_conds[tab];
        if (!single.empty()) {
            leaf = std::make_shared<FilterPlan>(std::move(leaf), single);
        }

        if (multi && !query->is_star) {
            std::vector<TabCol> need;
            auto add_col = [&](const TabCol &c) {
                if (c.tab_name != tab) return;
                for (auto &n : need) if (n.col_name == c.col_name) return;
                need.push_back(c);
            };
            for (auto &c : query->cols) add_col(c);
            for (auto &c : remaining_conds) {
                add_col(c.lhs_col);
                if (!c.is_rhs_val) add_col(c.rhs_col);
            }
            if (!need.empty()) {
                leaf = std::make_shared<ProjectionPlan>(T_Projection, std::move(leaf), need);
            }
        }

        if (!root) {
            root = leaf;
        } else {
            root = std::make_shared<JoinPlan>(T_NestLoop, std::move(root), leaf, join_conds);
        }
        joined.push_back(tab);
    }

    // 3.5) 处理剩余的连接条件（防御性处理，正常连通图不应有残留）
    if (!conds.empty()) {
        for (auto &c : conds) {
            push_conds(&c, root);
        }
        conds.clear();
    }

    // 4) 顶层投影（select 语句根一定是 Project）
    return std::make_shared<ProjectionPlan>(T_Projection, std::move(root), query->cols, query->is_star);
}



// 生成DDL语句和DML语句的查询执行计划
std::shared_ptr<Plan> Planner::do_planner(std::shared_ptr<Query> query, Context *context)
{
    std::shared_ptr<Plan> plannerRoot;
    if (auto x = std::dynamic_pointer_cast<ast::CreateTable>(query->parse)) {
        // create table;
        std::vector<ColDef> col_defs;
        for (auto &field : x->fields) {
            if (auto sv_col_def = std::dynamic_pointer_cast<ast::ColDef>(field)) {
                ColDef col_def = {.name = sv_col_def->col_name,
                                  .type = interp_sv_type(sv_col_def->type_len->type),
                                  .len = sv_col_def->type_len->len};
                col_defs.push_back(col_def);
            } else {
                throw InternalError("Unexpected field type");
            }
        }
        plannerRoot = std::make_shared<DDLPlan>(T_CreateTable, x->tab_name, std::vector<std::string>(), col_defs);
    } else if (auto x = std::dynamic_pointer_cast<ast::DropTable>(query->parse)) {
        // drop table;
        plannerRoot = std::make_shared<DDLPlan>(T_DropTable, x->tab_name, std::vector<std::string>(), std::vector<ColDef>());
    } else if (auto x = std::dynamic_pointer_cast<ast::CreateIndex>(query->parse)) {
        // create index;
        plannerRoot = std::make_shared<DDLPlan>(T_CreateIndex, x->tab_name, x->col_names, std::vector<ColDef>());
    } else if (auto x = std::dynamic_pointer_cast<ast::DropIndex>(query->parse)) {
        // drop index
        plannerRoot = std::make_shared<DDLPlan>(T_DropIndex, x->tab_name, x->col_names, std::vector<ColDef>());
    } else if (auto x = std::dynamic_pointer_cast<ast::InsertStmt>(query->parse)) {
        // insert;
        plannerRoot = std::make_shared<DMLPlan>(T_Insert, std::shared_ptr<Plan>(),  x->tab_name,  
                                                    query->values, std::vector<Condition>(), std::vector<SetClause>());
    } else if (auto x = std::dynamic_pointer_cast<ast::DeleteStmt>(query->parse)) {
        // delete;
        // 生成表扫描方式
        std::shared_ptr<Plan> table_scan_executors;
        // 只有一张表，不需要进行物理优化了
        // int index_no = get_indexNo(x->tab_name, query->conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(x->tab_name, query->conds, index_col_names);
        
        if (index_exist == false) {  // 该表没有索引
            index_col_names.clear();
            table_scan_executors = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        }

        plannerRoot = std::make_shared<DMLPlan>(T_Delete, table_scan_executors, x->tab_name,  
                                                std::vector<Value>(), query->conds, std::vector<SetClause>());
    } else if (auto x = std::dynamic_pointer_cast<ast::UpdateStmt>(query->parse)) {
        // update;
        // 生成表扫描方式
        std::shared_ptr<Plan> table_scan_executors;
        // 只有一张表，不需要进行物理优化了
        // int index_no = get_indexNo(x->tab_name, query->conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(x->tab_name, query->conds, index_col_names);

        if (index_exist == false) {  // 该表没有索引
        index_col_names.clear();
            table_scan_executors = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        }
        plannerRoot = std::make_shared<DMLPlan>(T_Update, table_scan_executors, x->tab_name,
                                                     std::vector<Value>(), query->conds, 
                                                     query->set_clauses);
    } else if (std::dynamic_pointer_cast<ast::SelectStmt>(query->parse) ||
               std::dynamic_pointer_cast<ast::UnionStmt>(query->parse)) {

        // 题目四：EXPLAIN ANALYZE 走独立的计划构建（谓词/投影下推 + 基数连接顺序）
        if (query->explain) {
            std::shared_ptr<Plan> projection = generate_explain_plan(query, context);
            plannerRoot = std::make_shared<DMLPlan>(T_explain, projection, std::string(), std::vector<Value>(),
                                                    std::vector<Condition>(), std::vector<SetClause>());
        } else {
            // 生成select语句的查询执行计划
            std::shared_ptr<Plan> projection = generate_select_plan(std::move(query), context);
            plannerRoot = std::make_shared<DMLPlan>(T_select, projection, std::string(), std::vector<Value>(),
                                                        std::vector<Condition>(), std::vector<SetClause>());
        }
    } else {
        throw InternalError("Unexpected AST root");
    }
    return plannerRoot;
}
