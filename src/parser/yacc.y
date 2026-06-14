%{
#include "ast.h"
#include "yacc.tab.h"
#include "errors.h"
#include <iostream>
#include <memory>
#include <stdexcept>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;
%}

// request a pure (reentrant) parser
%define api.pure full
// enable location in error handler
%locations
// enable verbose syntax error message
%define parse.error verbose

// keywords
%token SHOW TABLES CREATE TABLE DROP DESC INSERT INTO VALUES DELETE FROM ASC ORDER BY UNION
WHERE UPDATE SET SELECT INT CHAR FLOAT INDEX AND JOIN EXIT HELP TXN_BEGIN TXN_COMMIT TXN_ABORT TXN_ROLLBACK ORDER_BY ENABLE_NESTLOOP ENABLE_SORTMERGE
EXPLAIN ANALYZE AS ON GROUP HAVING LIMIT COUNT MAX MIN SUM AVG
// non-keywords
%token LEQ NEQ GEQ T_EOF

// type-specific tokens
%token <sv_str> IDENTIFIER VALUE_STRING
%token <sv_int> VALUE_INT
%token <sv_float> VALUE_FLOAT
%token <sv_bool> VALUE_BOOL

// specify types for non-terminal symbol
%type <sv_node> stmt dbStmt ddl dml txnStmt setStmt
%type <sv_field> field
%type <sv_fields> fieldList
%type <sv_type_len> type
%type <sv_comp_op> op
%type <sv_expr> expr
%type <sv_val> value
%type <sv_vals> valueList
%type <sv_str> tbName colName optAlias
%type <sv_strs> tableList colNameList
%type <sv_col> col
%type <sv_cols> colList group_clause opt_group_clause
%type <sv_select_items> selector selectList
%type <sv_select_item> selectItem
%type <sv_agg_expr> aggregate
%type <sv_having_cond> havingCondition
%type <sv_having_conds> havingClause opt_having_clause
%type <sv_set_clause> setClause
%type <sv_set_clauses> setClauses
%type <sv_cond> condition
%type <sv_conds> whereClause optWhereClause joinConds
%type <sv_orderby>  order_clause opt_order_clause
%type <sv_orderby_dir> opt_asc_desc
%type <sv_int> opt_limit_clause
%type <sv_setKnobType> set_knob_type
%type <sv_table_ref> tableRef
%type <sv_from_clause> fromClause
%type <sv_node> queryExpr selectStmt

%%
start:
        stmt ';'
    {
        parse_tree = $1;
        YYACCEPT;
    }
    |   HELP
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
    |   EXIT
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
    |   T_EOF
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
    ;

stmt:
        dbStmt
    |   ddl
    |   dml
    |   txnStmt
    |   setStmt
    ;

txnStmt:
        TXN_BEGIN
    {
        $$ = std::make_shared<TxnBegin>();
    }
    |   TXN_COMMIT
    {
        $$ = std::make_shared<TxnCommit>();
    }
    |   TXN_ABORT
    {
        $$ = std::make_shared<TxnAbort>();
    }
    | TXN_ROLLBACK
    {
        $$ = std::make_shared<TxnRollback>();
    }
    ;

dbStmt:
        SHOW TABLES
    {
        $$ = std::make_shared<ShowTables>();
    }
    |   SHOW INDEX FROM tbName
    {
        $$ = std::make_shared<ShowIndex>($4);
    }
    ;

setStmt:
        SET set_knob_type '=' VALUE_BOOL
    {
        $$ = std::make_shared<SetStmt>($2, $4);
    }
    ;

ddl:
        CREATE TABLE tbName '(' fieldList ')'
    {
        $$ = std::make_shared<CreateTable>($3, $5);
    }
    |   DROP TABLE tbName
    {
        $$ = std::make_shared<DropTable>($3);
    }
    |   DESC tbName
    {
        $$ = std::make_shared<DescTable>($2);
    }
    |   CREATE INDEX tbName '(' colNameList ')'
    {
        $$ = std::make_shared<CreateIndex>($3, $5);
    }
    |   DROP INDEX tbName '(' colNameList ')'
    {
        $$ = std::make_shared<DropIndex>($3, $5);
    }
    ;

dml:
        INSERT INTO tbName VALUES '(' valueList ')'
    {
        $$ = std::make_shared<InsertStmt>($3, $6);
    }
    |   DELETE FROM tbName optWhereClause
    {
        $$ = std::make_shared<DeleteStmt>($3, $4);
    }
    |   UPDATE tbName SET setClauses optWhereClause
    {
        $$ = std::make_shared<UpdateStmt>($2, $4, $5);
    }
    |   queryExpr
    {
        $$ = $1;
    }
    |   EXPLAIN ANALYZE queryExpr
    {
        auto sel = std::dynamic_pointer_cast<SelectStmt>($3);
        if (sel == nullptr) {
            throw RMDBError("failure");
        }
        $$ = std::make_shared<ExplainStmt>(sel);
    }
    ;

queryExpr:
        selectStmt
    {
        $$ = $1;
    }
    |   queryExpr UNION selectStmt
    {
        if (auto u = std::dynamic_pointer_cast<UnionStmt>($1)) {
            u->append($3);
            $$ = u;
        } else {
            $$ = std::make_shared<UnionStmt>($1, $3);
        }
    }
    ;

selectStmt:
        SELECT selector FROM fromClause optWhereClause opt_group_clause opt_having_clause opt_order_clause opt_limit_clause
    {
        std::vector<std::shared_ptr<Col>> legacy_cols;
        for (auto &item : $2) {
            if (auto c = std::dynamic_pointer_cast<Col>(item->expr)) {
                legacy_cols.push_back(c);
            }
        }
        auto sel = std::make_shared<SelectStmt>(legacy_cols, $2, std::vector<std::string>{}, $5, $8, $6, $7, $9);
        sel->from_refs = $4->refs;
        for (auto &ref : $4->refs) {
            sel->tabs.push_back(ref->tab_name);
            sel->tab_alias.push_back(ref->alias);
        }
        for (auto &c : $4->conds) {
            sel->conds.push_back(c);
        }
        $$ = sel;
    }
    ;

fieldList:
        field
    {
        $$ = std::vector<std::shared_ptr<Field>>{$1};
    }
    |   fieldList ',' field
    {
        $$.push_back($3);
    }
    ;

colNameList:
        colName
    {
        $$ = std::vector<std::string>{$1};
    }
    | colNameList ',' colName
    {
        $$.push_back($3);
    }
    ;

field:
        colName type
    {
        $$ = std::make_shared<ColDef>($1, $2);
    }
    ;

type:
        INT
    {
        $$ = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
    |   CHAR '(' VALUE_INT ')'
    {
        $$ = std::make_shared<TypeLen>(SV_TYPE_STRING, $3);
    }
    |   FLOAT
    {
        $$ = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
    ;

valueList:
        value
    {
        $$ = std::vector<std::shared_ptr<Value>>{$1};
    }
    |   valueList ',' value
    {
        $$.push_back($3);
    }
    ;

value:
        VALUE_INT
    {
        $$ = std::make_shared<IntLit>($1);
    }
    |   VALUE_FLOAT
    {
        $$ = std::make_shared<FloatLit>($1);
    }
    |   VALUE_STRING
    {
        $$ = std::make_shared<StringLit>($1);
    }
    |   VALUE_BOOL
    {
        $$ = std::make_shared<BoolLit>($1);
    }
    ;

condition:
        col op expr
    {
        $$ = std::make_shared<BinaryExpr>($1, $2, $3);
    }
    |   aggregate op expr
    {
        $$ = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), $2,
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
    |   col op aggregate
    {
        $$ = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), $2,
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
    |   aggregate op aggregate
    {
        $$ = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), $2,
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
    ;

joinConds:
        condition
    {
        $$ = std::vector<std::shared_ptr<BinaryExpr>>{$1};
    }
    |   joinConds AND condition
    {
        $$.push_back($3);
    }
    ;

optWhereClause:
        /* epsilon */ { /* ignore*/ }
    |   WHERE whereClause
    {
        $$ = $2;
    }
    ;

whereClause:
        condition 
    {
        $$ = std::vector<std::shared_ptr<BinaryExpr>>{$1};
    }
    |   whereClause AND condition
    {
        $$.push_back($3);
    }
    ;

col:
        tbName '.' colName
    {
        $$ = std::make_shared<Col>($1, $3);
    }
    |   colName
    {
        $$ = std::make_shared<Col>("", $1);
    }
    ;

colList:
        col
    {
        $$ = std::vector<std::shared_ptr<Col>>{$1};
    }
    |   colList ',' col
    {
        $$.push_back($3);
    }
    ;

op:
        '='
    {
        $$ = SV_OP_EQ;
    }
    |   '<'
    {
        $$ = SV_OP_LT;
    }
    |   '>'
    {
        $$ = SV_OP_GT;
    }
    |   NEQ
    {
        $$ = SV_OP_NE;
    }
    |   LEQ
    {
        $$ = SV_OP_LE;
    }
    |   GEQ
    {
        $$ = SV_OP_GE;
    }
    ;

expr:
        value
    {
        $$ = std::static_pointer_cast<Expr>($1);
    }
    |   col
    {
        $$ = std::static_pointer_cast<Expr>($1);
    }
    ;

setClauses:
        setClause
    {
        $$ = std::vector<std::shared_ptr<SetClause>>{$1};
    }
    |   setClauses ',' setClause
    {
        $$.push_back($3);
    }
    ;

setClause:
        colName '=' value
    {
        $$ = std::make_shared<SetClause>($1, $3);
    }
    ;

selector:
        '*'
    {
        $$ = {};
    }
    |   selectList
    ;

selectList:
        selectItem
    {
        $$ = std::vector<std::shared_ptr<SelectItem>>{$1};
    }
    |   selectList ',' selectItem
    {
        $$.push_back($3);
    }
    ;

selectItem:
        col optAlias
    {
        $$ = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>($1), $2);
    }
    |   aggregate optAlias
    {
        $$ = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>($1), $2);
    }
    ;

optAlias:
        /* epsilon */ { $$ = ""; }
    |   AS IDENTIFIER { $$ = $2; }
    |   IDENTIFIER { $$ = $1; }
    ;

aggregate:
        COUNT '(' '*' ')'
    {
        $$ = std::make_shared<AggExpr>(Agg_COUNT, nullptr, true);
    }
    |   COUNT '(' col ')'
    {
        $$ = std::make_shared<AggExpr>(Agg_COUNT, $3, false);
    }
    |   MAX '(' col ')'
    {
        $$ = std::make_shared<AggExpr>(Agg_MAX, $3, false);
    }
    |   MIN '(' col ')'
    {
        $$ = std::make_shared<AggExpr>(Agg_MIN, $3, false);
    }
    |   SUM '(' col ')'
    {
        $$ = std::make_shared<AggExpr>(Agg_SUM, $3, false);
    }
    |   AVG '(' col ')'
    {
        $$ = std::make_shared<AggExpr>(Agg_AVG, $3, false);
    }
    ;

tableList:
        tbName
    {
        $$ = std::vector<std::string>{$1};
    }
    |   tableList ',' tbName
    {
        $$.push_back($3);
    }
    |   tableList JOIN tbName
    {
        $$.push_back($3);
    }
    ;

/* 题目四：表引用，支持可选别名 customers c / customers AS c */
tableRef:
        tbName
    {
        $$ = std::make_shared<TableRef>($1);
    }
    |   tbName IDENTIFIER
    {
        $$ = std::make_shared<TableRef>($1, $2);
    }
    |   tbName AS IDENTIFIER
    {
        $$ = std::make_shared<TableRef>($1, $3);
    }
    |   '(' queryExpr ')' AS IDENTIFIER
    {
        $$ = std::make_shared<TableRef>($2, $5);
    }
    |   '(' queryExpr ')' IDENTIFIER
    {
        $$ = std::make_shared<TableRef>($2, $4);
    }
    ;

/* 题目四：FROM 子句，支持逗号连接、JOIN（裸）以及 JOIN ... ON condition */
fromClause:
        tableRef
    {
        $$ = std::make_shared<FromClause>();
        $$->refs.push_back($1);
    }
    |   fromClause ',' tableRef
    {
        $$ = $1;
        $$->refs.push_back($3);
    }
    |   fromClause JOIN tableRef ON joinConds
    {
        $$ = $1;
        $$->refs.push_back($3);
        for (auto &cond : $5) {
            $$->conds.push_back(cond);
        }
    }
    |   fromClause JOIN tableRef
    {
        $$ = $1;
        $$->refs.push_back($3);
    }
    ;

opt_order_clause:
    ORDER BY order_clause      
    { 
        $$ = $3; 
    }
    |   /* epsilon */ { /* ignore*/ }
    ;

order_clause:
      col  opt_asc_desc 
    { 
        $$ = std::make_shared<OrderBy>($1, $2);
    }
    | order_clause ',' col opt_asc_desc
    {
        $$ = $1;
        $$->append($3, $4);
    }
    ;   

opt_asc_desc:
    ASC          { $$ = OrderBy_ASC;     }
    |  DESC      { $$ = OrderBy_DESC;    }
    |       { $$ = OrderBy_DEFAULT; }
    ;    

opt_group_clause:
        /* epsilon */ { $$ = {}; }
    |   GROUP BY group_clause
    {
        $$ = $3;
    }
    ;

group_clause:
        col
    {
        $$ = std::vector<std::shared_ptr<Col>>{$1};
    }
    |   group_clause ',' col
    {
        $$.push_back($3);
    }
    ;

opt_having_clause:
        /* epsilon */ { $$ = {}; }
    |   HAVING havingClause
    {
        $$ = $2;
    }
    ;

havingClause:
        havingCondition
    {
        $$ = std::vector<std::shared_ptr<HavingExpr>>{$1};
    }
    |   havingClause AND havingCondition
    {
        $$.push_back($3);
    }
    ;

havingCondition:
        expr op expr
    {
        $$ = std::make_shared<HavingExpr>($1, $2, $3);
    }
    |   aggregate op expr
    {
        $$ = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>($1), $2, $3);
    }
    |   expr op aggregate
    {
        $$ = std::make_shared<HavingExpr>($1, $2, std::static_pointer_cast<Expr>($3));
    }
    |   aggregate op aggregate
    {
        $$ = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>($1), $2, std::static_pointer_cast<Expr>($3));
    }
    ;

opt_limit_clause:
        /* epsilon */ { $$ = -1; }
    |   LIMIT VALUE_INT { $$ = $2; }
    ;

set_knob_type:
    ENABLE_NESTLOOP { $$ = EnableNestLoop; }
    |   ENABLE_SORTMERGE { $$ = EnableSortMerge; }
    ;

tbName: IDENTIFIER;

colName: IDENTIFIER;
%%
