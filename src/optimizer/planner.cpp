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

bool Planner::get_index_cols(std::string tab_name, std::vector<Condition> curr_conds, std::vector<std::string>& index_col_names) {
    index_col_names.clear();
    TabMeta& tab = sm_manager_->db_.get_table(tab_name);
    size_t best_match = 0;
    std::vector<std::string> best_cols;
    for (auto &index : tab.indexes) {
        size_t matched = 0;
        for (auto &col : index.cols) {
            bool found = false;
            for (auto &cond : curr_conds) {
                if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name && cond.lhs_col.col_name == col.name &&
                    cond.op != OP_NE) {
                    found = true;
                    break;
                }
            }
            if (!found) break;
            matched++;
            bool has_range = false;
            for (auto &cond : curr_conds) {
                if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name && cond.lhs_col.col_name == col.name &&
                    cond.op != OP_EQ && cond.op != OP_NE) {
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
            std::map<CompOp, CompOp> swap_op = {
                {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
            };
            std::swap(cond->lhs_col, cond->rhs_col);
            cond->op = swap_op.at(cond->op);
        }
        x->conds_.emplace_back(std::move(*cond));
        return 3;
    }
    return false;
}

std::shared_ptr<Plan> pop_scan(int *scantbl, std::string table, std::vector<std::string> &joined_tables, 
                std::vector<std::shared_ptr<Plan>> plans)
{
    for (size_t i = 0; i < plans.size(); i++) {
        auto x = std::dynamic_pointer_cast<ScanPlan>(plans[i]);
        if(x->tab_name_.compare(table) == 0)
        {
            scantbl[i] = 1;
            joined_tables.emplace_back(x->tab_name_);
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

std::shared_ptr<Plan> Planner::physical_optimization(std::shared_ptr<Query> query, Context *context)
{
    std::shared_ptr<Plan> plan = make_one_rel(query);
    
    // 其他物理优化

    // 处理orderby
    plan = generate_sort_plan(query, std::move(plan)); 

    return plan;
}



std::shared_ptr<Plan> Planner::make_one_rel(std::shared_ptr<Query> query)
{
    auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse);
    std::vector<std::string> tables = query->tables;
    // // Scan table , 生成表算子列表tab_nodes
    std::vector<std::shared_ptr<Plan>> table_scan_executors(tables.size());
    for (size_t i = 0; i < tables.size(); i++) {
        auto curr_conds = pop_conds(query->conds, tables[i]);
        // int index_no = get_indexNo(tables[i], curr_conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(tables[i], curr_conds, index_col_names);
        if (index_exist == false) {  // 该表没有索引
            index_col_names.clear();
            table_scan_executors[i] = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tables[i], curr_conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors[i] =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, tables[i], curr_conds, index_col_names);
        }
    }
    // 只有一个表，不需要join。
    if(tables.size() == 1)
    {
        return table_scan_executors[0];
    }
    // 获取where条件
    auto conds = std::move(query->conds);
    std::shared_ptr<Plan> table_join_executors;
    
    int scantbl[tables.size()];
    for(size_t i = 0; i < tables.size(); i++)
    {
        scantbl[i] = -1;
    }
    // 假设在ast中已经添加了jointree，这里需要修改的逻辑是，先处理jointree，然后再考虑剩下的部分
    if(conds.size() >= 1)
    {
        // 有连接条件

        // 根据连接条件，生成第一层join
        std::vector<std::string> joined_tables(tables.size());
        auto it = conds.begin();
        while (it != conds.end()) {
            std::shared_ptr<Plan> left , right;
            left = pop_scan(scantbl, it->lhs_col.tab_name, joined_tables, table_scan_executors);
            right = pop_scan(scantbl, it->rhs_col.tab_name, joined_tables, table_scan_executors);
            std::vector<Condition> join_conds{*it};
            //建立join
            // 判断使用哪种join方式
            if(enable_nestedloop_join && enable_sortmerge_join) {
                // 默认nested loop join
                table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(left), std::move(right), join_conds);
            } else if(enable_nestedloop_join) {
                table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(left), std::move(right), join_conds);
            } else if(enable_sortmerge_join) {
                table_join_executors = std::make_shared<JoinPlan>(T_SortMerge, std::move(left), std::move(right), join_conds);
            } else {
                // error
                throw RMDBError("No join executor selected!");
            }

            // table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(left), std::move(right), join_conds);
            it = conds.erase(it);
            break;
        }
        // 根据连接条件，生成第2-n层join
        it = conds.begin();
        while (it != conds.end()) {
            std::shared_ptr<Plan> left_need_to_join_executors = nullptr;
            std::shared_ptr<Plan> right_need_to_join_executors = nullptr;
            bool isneedreverse = false;
            if (std::find(joined_tables.begin(), joined_tables.end(), it->lhs_col.tab_name) == joined_tables.end()) {
                left_need_to_join_executors = pop_scan(scantbl, it->lhs_col.tab_name, joined_tables, table_scan_executors);
            }
            if (std::find(joined_tables.begin(), joined_tables.end(), it->rhs_col.tab_name) == joined_tables.end()) {
                right_need_to_join_executors = pop_scan(scantbl, it->rhs_col.tab_name, joined_tables, table_scan_executors);
                isneedreverse = true;
            } 

            if(left_need_to_join_executors != nullptr && right_need_to_join_executors != nullptr) {
                std::vector<Condition> join_conds{*it};
                std::shared_ptr<Plan> temp_join_executors = std::make_shared<JoinPlan>(T_NestLoop, 
                                                                    std::move(left_need_to_join_executors), 
                                                                    std::move(right_need_to_join_executors), 
                                                                    join_conds);
                table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(temp_join_executors), 
                                                                    std::move(table_join_executors), 
                                                                    std::vector<Condition>());
            } else if(left_need_to_join_executors != nullptr || right_need_to_join_executors != nullptr) {
                if(isneedreverse) {
                    std::map<CompOp, CompOp> swap_op = {
                        {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
                    };
                    std::swap(it->lhs_col, it->rhs_col);
                    it->op = swap_op.at(it->op);
                    left_need_to_join_executors = std::move(right_need_to_join_executors);
                }
                std::vector<Condition> join_conds{*it};
                table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(left_need_to_join_executors), 
                                                                    std::move(table_join_executors), join_conds);
            } else {
                push_conds(std::move(&(*it)), table_join_executors);
            }
            it = conds.erase(it);
        }
    } else {
        table_join_executors = table_scan_executors[0];
        scantbl[0] = 1;
    }

    //连接剩余表
    for (size_t i = 0; i < tables.size(); i++) {
        if(scantbl[i] == -1) {
            table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(table_scan_executors[i]), 
                                                    std::move(table_join_executors), std::vector<Condition>());
        }
    }

    return table_join_executors;

}


std::shared_ptr<Plan> Planner::generate_sort_plan(std::shared_ptr<Query> query, std::shared_ptr<Plan> plan)
{
    auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse);
    if(!x->has_sort) {
        return plan;
    }
    std::vector<std::string> tables = query->tables;
    std::vector<ColMeta> all_cols;
    for (auto &sel_tab_name : tables) {
        // 这里db_不能写成get_db(), 注意要传指针
        const auto &sel_tab_cols = sm_manager_->db_.get_table(sel_tab_name).cols;
        all_cols.insert(all_cols.end(), sel_tab_cols.begin(), sel_tab_cols.end());
    }
    TabCol sel_col;
    for (auto &col : all_cols) {
        if(col.name.compare(x->order->cols->col_name) == 0 )
        sel_col = {.tab_name = col.tab_name, .col_name = col.name};
    }
    return std::make_shared<SortPlan>(T_Sort, std::move(plan), sel_col, 
                                    x->order->orderby_dir == ast::OrderBy_DESC);
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

// 取出属于单表的谓词（左列属于该表且右值为常量，或左右列同属一表）
static std::vector<Condition> explain_pop_single_conds(std::vector<Condition> &conds, const std::string &tab) {
    std::vector<Condition> solved;
    auto it = conds.begin();
    while (it != conds.end()) {
        if ((it->lhs_col.tab_name == tab && it->is_rhs_val) ||
            (!it->is_rhs_val && it->lhs_col.tab_name == tab && it->rhs_col.tab_name == tab)) {
            solved.emplace_back(std::move(*it));
            it = conds.erase(it);
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
    } else if (auto x = std::dynamic_pointer_cast<JoinPlan>(plan)) {
        explain_collect_tables(x->left_, out);
        explain_collect_tables(x->right_, out);
    }
}

// 判断条件涉及的表是否都已包含在给定表集合中
static bool conds_tables_covered(const Condition &c, const std::vector<std::string> &tabs) {
    auto has = [&](const std::string &t) {
        return std::find(tabs.begin(), tabs.end(), t) != tabs.end();
    };
    if (c.is_rhs_val) return has(c.lhs_col.tab_name);
    return has(c.lhs_col.tab_name) && has(c.rhs_col.tab_name);
}

std::shared_ptr<Plan> Planner::generate_explain_plan(std::shared_ptr<Query> query, Context *context) {
    std::vector<std::string> tables = query->tables;
    std::vector<Condition> conds = query->conds;  // 拷贝，下面会消耗

    // 1) 谓词下推：先把每张表的单表条件全部取出，剩下的 conds 即为跨表连接条件。
    //    必须先全部取出，否则投影下推时仍会把别的表尚未处理的单表谓词列误并入投影。
    struct Leaf { std::shared_ptr<Plan> plan; std::string tab; size_t card; };
    std::vector<Leaf> leaves;
    bool multi = tables.size() > 1;

    std::map<std::string, std::vector<Condition>> single_conds;
    for (auto &tab : tables) {
        single_conds[tab] = explain_pop_single_conds(conds, tab);
    }
    // 此时 conds 仅剩连接条件，用于决定投影下推应保留哪些列
    std::vector<Condition> &join_conds = conds;

    // 2) 为每张表构建：Scan(空谓词) -> 可选 Filter -> 可选 Project(投影下推)
    for (auto &tab : tables) {
        // 空谓词扫描（rows = 扫描到的全部行）
        std::shared_ptr<Plan> leaf =
            std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tab, std::vector<Condition>(), std::vector<std::string>());
        auto &single = single_conds[tab];
        if (!single.empty()) {
            leaf = std::make_shared<FilterPlan>(std::move(leaf), single);
        }
        // 投影下推：仅在多表连接且非 SELECT * 时进行。
        // 保留的列 = select 列 + 连接条件列；单表过滤谓词的列在 Filter 中已消费，无需投影。
        if (multi && !query->is_star) {
            std::vector<TabCol> need;
            auto add_col = [&](const TabCol &c) {
                if (c.tab_name != tab) return;
                for (auto &n : need) if (n.col_name == c.col_name) return;
                need.push_back(c);
            };
            for (auto &c : query->cols) add_col(c);            // select 需要的列
            for (auto &c : join_conds) {                       // 连接条件需要的列
                add_col(c.lhs_col);
                if (!c.is_rhs_val) add_col(c.rhs_col);
            }
            // 与参考实现一致：仅当投影确实减少了列数时才插入 Project 节点；
            // 若需要的列覆盖了该表全部列，则不下推投影。
            size_t tab_col_num = sm_manager_->db_.get_table(tab).cols.size();
            if (!need.empty() && need.size() != tab_col_num) {
                leaf = std::make_shared<ProjectionPlan>(T_Projection, std::move(leaf), need);
            }
        }
        leaves.push_back({std::move(leaf), tab, get_table_cardinality(tab)});
    }

    // 3) 基于基数升序排序（最小表作最左外表）
    std::stable_sort(leaves.begin(), leaves.end(),
                     [](const Leaf &a, const Leaf &b) { return a.card < b.card; });

    // 3) 折叠为左深连接树（连接顺序优化）
    // 贪心：从基数最小的表出发，每一步在剩余表中优先挑选“能与已连接集合通过连接条件相连”的最小基数表，
    // 避免凭空产生笛卡尔积（如 a、b 都只与 c 相连时，不能先把 a、b 直接连成空条件的 Join）。
    // 仅当没有任何可连接表时，才退而连接剩余最小基数的表。
    std::vector<bool> used(leaves.size(), false);
    std::shared_ptr<Plan> root = leaves.empty() ? nullptr : leaves[0].plan;
    std::vector<std::string> joined;
    if (!leaves.empty()) { joined.push_back(leaves[0].tab); used[0] = true; }

    // 判断候选表是否能与当前已连接集合直接相连
    auto can_join = [&](const std::string &cand) {
        for (auto &c : conds) {
            if (c.is_rhs_val) continue;
            bool lhs_cand = c.lhs_col.tab_name == cand;
            bool rhs_cand = c.rhs_col.tab_name == cand;
            bool lhs_in = std::find(joined.begin(), joined.end(), c.lhs_col.tab_name) != joined.end();
            bool rhs_in = std::find(joined.begin(), joined.end(), c.rhs_col.tab_name) != joined.end();
            if ((lhs_cand && rhs_in) || (rhs_cand && lhs_in)) return true;
        }
        return false;
    };

    for (size_t step = 1; step < leaves.size(); step++) {
        // leaves 已按基数升序排列，正序扫描即为“最小基数优先”
        int pick = -1;
        for (size_t i = 0; i < leaves.size(); i++) {
            if (!used[i] && can_join(leaves[i].tab)) { pick = (int)i; break; }
        }
        if (pick < 0) {  // 无可连接表，退而取剩余最小基数表（产生笛卡尔积）
            for (size_t i = 0; i < leaves.size(); i++) {
                if (!used[i]) { pick = (int)i; break; }
            }
        }
        used[pick] = true;
        std::vector<std::string> next_tabs = joined;
        next_tabs.push_back(leaves[pick].tab);
        // 选出此次连接可用的条件（两侧表都已就绪）
        std::vector<Condition> jc;
        auto it = conds.begin();
        while (it != conds.end()) {
            if (conds_tables_covered(*it, next_tabs)) {
                jc.push_back(*it);
                it = conds.erase(it);
            } else {
                ++it;
            }
        }
        root = std::make_shared<JoinPlan>(T_NestLoop, std::move(root), leaves[pick].plan, jc);
        joined.push_back(leaves[pick].tab);
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
    } else if (auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse)) {

        std::shared_ptr<plannerInfo> root = std::make_shared<plannerInfo>(x);
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
