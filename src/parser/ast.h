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
#include <string>
#include <memory>

enum JoinType {
    INNER_JOIN, LEFT_JOIN, RIGHT_JOIN, FULL_JOIN
};
namespace ast {

enum SvType {
    SV_TYPE_INT, SV_TYPE_FLOAT, SV_TYPE_STRING, SV_TYPE_BOOL
};

enum SvCompOp {
    SV_OP_EQ, SV_OP_NE, SV_OP_LT, SV_OP_GT, SV_OP_LE, SV_OP_GE
};

enum OrderByDir {
    OrderBy_DEFAULT,
    OrderBy_ASC,
    OrderBy_DESC
};

enum AggFuncType {
    Agg_COUNT,
    Agg_MAX,
    Agg_MIN,
    Agg_SUM,
    Agg_AVG
};

enum SetKnobType {
    EnableNestLoop, EnableSortMerge
};

// Base class for tree nodes
struct TreeNode {
    virtual ~TreeNode() = default;  // enable polymorphism
};

struct Help : public TreeNode {
};

struct ShowTables : public TreeNode {
};

struct ShowIndex : public TreeNode {
    std::string tab_name;

    ShowIndex(std::string tab_name_) : tab_name(std::move(tab_name_)) {}
};

struct TxnBegin : public TreeNode {
};

struct TxnCommit : public TreeNode {
};

struct TxnAbort : public TreeNode {
};

struct TxnRollback : public TreeNode {
};

struct TypeLen : public TreeNode {
    SvType type;
    int len;

    TypeLen(SvType type_, int len_) : type(type_), len(len_) {}
};

struct Field : public TreeNode {
};

struct ColDef : public Field {
    std::string col_name;
    std::shared_ptr<TypeLen> type_len;

    ColDef(std::string col_name_, std::shared_ptr<TypeLen> type_len_) :
            col_name(std::move(col_name_)), type_len(std::move(type_len_)) {}
};

struct CreateTable : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<Field>> fields;

    CreateTable(std::string tab_name_, std::vector<std::shared_ptr<Field>> fields_) :
            tab_name(std::move(tab_name_)), fields(std::move(fields_)) {}
};

struct DropTable : public TreeNode {
    std::string tab_name;

    DropTable(std::string tab_name_) : tab_name(std::move(tab_name_)) {}
};

struct DescTable : public TreeNode {
    std::string tab_name;

    DescTable(std::string tab_name_) : tab_name(std::move(tab_name_)) {}
};

struct CreateIndex : public TreeNode {
    std::string tab_name;
    std::vector<std::string> col_names;

    CreateIndex(std::string tab_name_, std::vector<std::string> col_names_) :
            tab_name(std::move(tab_name_)), col_names(std::move(col_names_)) {}
};

struct DropIndex : public TreeNode {
    std::string tab_name;
    std::vector<std::string> col_names;

    DropIndex(std::string tab_name_, std::vector<std::string> col_names_) :
            tab_name(std::move(tab_name_)), col_names(std::move(col_names_)) {}
};

struct Expr : public TreeNode {
};

struct Value : public Expr {
};

struct IntLit : public Value {
    int val;

    IntLit(int val_) : val(val_) {}
};

struct FloatLit : public Value {
    float val;

    FloatLit(float val_) : val(val_) {}
};

struct StringLit : public Value {
    std::string val;

    StringLit(std::string val_) : val(std::move(val_)) {}
};

struct BoolLit : public Value {
    bool val;

    BoolLit(bool val_) : val(val_) {}
};

struct Col : public Expr {
    std::string tab_name;
    std::string col_name;

    Col(std::string tab_name_, std::string col_name_) :
            tab_name(std::move(tab_name_)), col_name(std::move(col_name_)) {}
};

struct AggExpr : public Expr {
    AggFuncType func;
    std::shared_ptr<Col> col;
    bool is_star;

    AggExpr(AggFuncType func_, std::shared_ptr<Col> col_, bool is_star_) :
            func(func_), col(std::move(col_)), is_star(is_star_) {}
};

struct SelectItem : public TreeNode {
    std::shared_ptr<Expr> expr;
    std::string alias;

    SelectItem(std::shared_ptr<Expr> expr_, std::string alias_ = "") :
            expr(std::move(expr_)), alias(std::move(alias_)) {}
};

struct SetClause : public TreeNode {
    std::string col_name;
    std::shared_ptr<Value> val;

    SetClause(std::string col_name_, std::shared_ptr<Value> val_) :
            col_name(std::move(col_name_)), val(std::move(val_)) {}
};

struct BinaryExpr : public TreeNode {
    std::shared_ptr<Col> lhs;
    SvCompOp op;
    std::shared_ptr<Expr> rhs;

    BinaryExpr(std::shared_ptr<Col> lhs_, SvCompOp op_, std::shared_ptr<Expr> rhs_) :
            lhs(std::move(lhs_)), op(op_), rhs(std::move(rhs_)) {}
};

struct HavingExpr : public TreeNode {
    std::shared_ptr<Expr> lhs;
    SvCompOp op;
    std::shared_ptr<Expr> rhs;

    HavingExpr(std::shared_ptr<Expr> lhs_, SvCompOp op_, std::shared_ptr<Expr> rhs_) :
            lhs(std::move(lhs_)), op(op_), rhs(std::move(rhs_)) {}
};

struct OrderBy : public TreeNode
{
    std::vector<std::shared_ptr<Col>> cols;
    std::vector<OrderByDir> orderby_dirs;

    OrderBy(std::shared_ptr<Col> col_, OrderByDir orderby_dir_) {
        cols.push_back(std::move(col_));
        orderby_dirs.push_back(orderby_dir_);
    }

    void append(std::shared_ptr<Col> col_, OrderByDir orderby_dir_) {
        cols.push_back(std::move(col_));
        orderby_dirs.push_back(orderby_dir_);
    }
};

struct InsertStmt : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<Value>> vals;

    InsertStmt(std::string tab_name_, std::vector<std::shared_ptr<Value>> vals_) :
            tab_name(std::move(tab_name_)), vals(std::move(vals_)) {}
};

struct DeleteStmt : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<BinaryExpr>> conds;

    DeleteStmt(std::string tab_name_, std::vector<std::shared_ptr<BinaryExpr>> conds_) :
            tab_name(std::move(tab_name_)), conds(std::move(conds_)) {}
};

struct UpdateStmt : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<SetClause>> set_clauses;
    std::vector<std::shared_ptr<BinaryExpr>> conds;

    UpdateStmt(std::string tab_name_,
               std::vector<std::shared_ptr<SetClause>> set_clauses_,
               std::vector<std::shared_ptr<BinaryExpr>> conds_) :
            tab_name(std::move(tab_name_)), set_clauses(std::move(set_clauses_)), conds(std::move(conds_)) {}
};

struct JoinExpr : public TreeNode {
    std::string left;
    std::string right;
    std::vector<std::shared_ptr<BinaryExpr>> conds;
    JoinType type;

    JoinExpr(std::string left_, std::string right_,
               std::vector<std::shared_ptr<BinaryExpr>> conds_, JoinType type_) :
            left(std::move(left_)), right(std::move(right_)), conds(std::move(conds_)), type(type_) {}
};

// 题目四：表引用，支持可选的表别名（FROM customers c）
struct TableRef : public TreeNode {
    std::string tab_name;   // 真实表名
    std::string alias;      // 表别名，没有则为空串
    std::shared_ptr<TreeNode> derived_query;  // 题目六：派生表子查询
    bool is_derived = false;

    TableRef(std::string tab_name_, std::string alias_ = "") :
            tab_name(std::move(tab_name_)), alias(std::move(alias_)) {}

    TableRef(std::shared_ptr<TreeNode> derived_query_, std::string alias_) :
            alias(std::move(alias_)), derived_query(std::move(derived_query_)), is_derived(true) {}
};

// 题目四：FROM 子句，收集表引用以及 JOIN ... ON 产生的连接条件
struct FromClause : public TreeNode {
    std::vector<std::shared_ptr<TableRef>> refs;       // 表引用列表（含别名）
    std::vector<std::shared_ptr<BinaryExpr>> conds;    // JOIN ON 收集到的连接条件
};

struct SelectStmt : public TreeNode {
    std::vector<std::shared_ptr<Col>> cols;
    std::vector<std::shared_ptr<SelectItem>> items;
    std::vector<std::string> tabs;
    std::vector<std::string> tab_alias;   // 题目四：与 tabs 平行的别名列表，无别名为空串
    std::vector<std::shared_ptr<BinaryExpr>> conds;
    std::vector<std::shared_ptr<JoinExpr>> jointree;
    std::vector<std::shared_ptr<TableRef>> from_refs;  // 题目六：真实表/派生表统一 FROM 源
    std::vector<std::shared_ptr<Col>> group_cols;
    std::vector<std::shared_ptr<HavingExpr>> having_conds;


    bool has_sort;
    std::shared_ptr<OrderBy> order;
    int limit;


    SelectStmt(std::vector<std::shared_ptr<Col>> cols_,
               std::vector<std::shared_ptr<SelectItem>> items_,
               std::vector<std::string> tabs_,
               std::vector<std::shared_ptr<BinaryExpr>> conds_,
               std::shared_ptr<OrderBy> order_,
               std::vector<std::shared_ptr<Col>> group_cols_ = {},
               std::vector<std::shared_ptr<HavingExpr>> having_conds_ = {},
               int limit_ = -1) :
            cols(std::move(cols_)), items(std::move(items_)), tabs(std::move(tabs_)), conds(std::move(conds_)),
            group_cols(std::move(group_cols_)), having_conds(std::move(having_conds_)),
            order(std::move(order_)), limit(limit_) {
                has_sort = (bool)order;
            }
};

struct UnionStmt : public TreeNode {
    std::vector<std::shared_ptr<TreeNode>> branches;

    UnionStmt(std::shared_ptr<TreeNode> lhs, std::shared_ptr<TreeNode> rhs) {
        branches.push_back(std::move(lhs));
        branches.push_back(std::move(rhs));
    }

    void append(std::shared_ptr<TreeNode> branch) {
        branches.push_back(std::move(branch));
    }
};

// 题目四：EXPLAIN ANALYZE 语句，包裹内层的 SELECT 语句
struct ExplainStmt : public TreeNode {
    std::shared_ptr<SelectStmt> sel;

    ExplainStmt(std::shared_ptr<SelectStmt> sel_) : sel(std::move(sel_)) {}
};

// set enable_nestloop
struct SetStmt : public TreeNode {
    SetKnobType set_knob_type_;
    bool bool_val_;

    SetStmt(SetKnobType &type, bool bool_value) : 
        set_knob_type_(type), bool_val_(bool_value) { }
};

// Semantic value
struct SemValue {
    int sv_int;
    float sv_float;
    std::string sv_str;
    bool sv_bool;
    OrderByDir sv_orderby_dir;
    std::vector<std::string> sv_strs;

    std::shared_ptr<TreeNode> sv_node;

    SvCompOp sv_comp_op;

    std::shared_ptr<TypeLen> sv_type_len;

    std::shared_ptr<Field> sv_field;
    std::vector<std::shared_ptr<Field>> sv_fields;

    std::shared_ptr<Expr> sv_expr;

    std::shared_ptr<Value> sv_val;
    std::vector<std::shared_ptr<Value>> sv_vals;

    std::shared_ptr<Col> sv_col;
    std::vector<std::shared_ptr<Col>> sv_cols;
    std::shared_ptr<AggExpr> sv_agg_expr;
    std::shared_ptr<SelectItem> sv_select_item;
    std::vector<std::shared_ptr<SelectItem>> sv_select_items;
    std::shared_ptr<HavingExpr> sv_having_cond;
    std::vector<std::shared_ptr<HavingExpr>> sv_having_conds;

    std::shared_ptr<SetClause> sv_set_clause;
    std::vector<std::shared_ptr<SetClause>> sv_set_clauses;

    std::shared_ptr<BinaryExpr> sv_cond;
    std::vector<std::shared_ptr<BinaryExpr>> sv_conds;

    std::shared_ptr<OrderBy> sv_orderby;

    SetKnobType sv_setKnobType;

    // 题目四：表引用（含别名）与 FROM 子句
    std::shared_ptr<TableRef> sv_table_ref;
    std::shared_ptr<FromClause> sv_from_clause;
};

extern std::shared_ptr<ast::TreeNode> parse_tree;

}

#define YYSTYPE ast::SemValue
