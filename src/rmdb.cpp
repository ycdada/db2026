/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <netinet/in.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "errors.h"
#include "optimizer/optimizer.h"
#include "recovery/log_recovery.h"
#include "optimizer/plan.h"
#include "optimizer/planner.h"
#include "portal.h"
#include "analyze/analyze.h"

#define SOCK_PORT 8765
#define MAX_CONN_LIMIT 8

static bool should_exit = false;

// 构建全局所需的管理器对象
auto disk_manager = std::make_unique<DiskManager>();
auto buffer_pool_manager = std::make_unique<BufferPoolManager>(BUFFER_POOL_SIZE, disk_manager.get());
auto rm_manager = std::make_unique<RmManager>(disk_manager.get(), buffer_pool_manager.get());
auto ix_manager = std::make_unique<IxManager>(disk_manager.get(), buffer_pool_manager.get());
auto sm_manager = std::make_unique<SmManager>(disk_manager.get(), buffer_pool_manager.get(), rm_manager.get(), ix_manager.get());
auto lock_manager = std::make_unique<LockManager>();
auto txn_manager = std::make_unique<TransactionManager>(lock_manager.get(), sm_manager.get());
auto planner = std::make_unique<Planner>(sm_manager.get());
auto optimizer = std::make_unique<Optimizer>(sm_manager.get(), planner.get());
auto ql_manager = std::make_unique<QlManager>(sm_manager.get(), txn_manager.get(), planner.get());
auto log_manager = std::make_unique<LogManager>(disk_manager.get());
auto recovery = std::make_unique<RecoveryManager>(disk_manager.get(), buffer_pool_manager.get(), sm_manager.get());
auto portal = std::make_unique<Portal>(sm_manager.get());
auto analyze = std::make_unique<Analyze>(sm_manager.get());
std::atomic<bool> isolation_output_format{false};
pthread_mutex_t *buffer_mutex;

static jmp_buf jmpbuf;
void sigint_handler(int signo) {
    should_exit = true;
    log_manager->flush_log_to_disk();
    std::cout << "The Server receive Crtl+C, will been closed\n";
    longjmp(jmpbuf, 1);
}

namespace {

constexpr size_t PLAN_CACHE_MAX_ENTRIES = 256;

struct CachedPlan {
    uint64_t version;
    std::shared_ptr<Plan> plan;
};

struct TemplateLiteral {
    bool is_string = false;
    std::string raw;
    std::string display;
};

struct SqlTemplate {
    bool ok = false;
    std::string key;
    std::vector<TemplateLiteral> literals;
};

struct TemplateValueSpec {
    ColType type = TYPE_INT;
    int raw_len = 0;
    bool initialized = false;
};

struct CachedTemplatePlan {
    uint64_t version;
    std::shared_ptr<Plan> plan;
    std::vector<TemplateValueSpec> specs;
};

std::mutex plan_cache_latch;
std::unordered_map<std::string, CachedPlan> plan_cache;
std::unordered_map<std::string, CachedTemplatePlan> template_plan_cache;
std::atomic<uint64_t> plan_cache_version{0};

bool is_word_char(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

void append_normalized_token(std::string &out, std::string_view token, bool word_token) {
    if (token.empty()) {
        return;
    }
    if (word_token && !out.empty() && is_word_char(out.back())) {
        out.push_back(' ');
    }
    out.append(token.data(), token.size());
}

bool sign_belongs_to_number(std::string_view sql, size_t pos) {
    if (pos + 1 >= sql.size() || !std::isdigit(static_cast<unsigned char>(sql[pos + 1]))) {
        return false;
    }
    size_t prev = pos;
    while (prev > 0) {
        --prev;
        if (!std::isspace(static_cast<unsigned char>(sql[prev]))) {
            char ch = sql[prev];
            return ch == '(' || ch == ',' || ch == '=' || ch == '<' || ch == '>' || ch == '!';
        }
    }
    return true;
}

SqlTemplate build_sql_template(std::string_view sql) {
    SqlTemplate result;
    if (!sql.empty() && sql.back() == ';') {
        sql.remove_suffix(1);
    }
    size_t i = 0;
    while (i < sql.size()) {
        unsigned char ch = static_cast<unsigned char>(sql[i]);
        if (std::isspace(ch)) {
            ++i;
            continue;
        }
        if (std::isalpha(ch) || sql[i] == '_') {
            size_t begin = i;
            while (i < sql.size() && is_word_char(sql[i])) {
                ++i;
            }
            std::string word(sql.substr(begin, i - begin));
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            append_normalized_token(result.key, word, true);
            continue;
        }
        if (std::isdigit(ch) || ((sql[i] == '+' || sql[i] == '-') && sign_belongs_to_number(sql, i))) {
            size_t begin = i;
            if (sql[i] == '+' || sql[i] == '-') {
                ++i;
            }
            while (i < sql.size() && std::isdigit(static_cast<unsigned char>(sql[i]))) {
                ++i;
            }
            if (i < sql.size() && sql[i] == '.') {
                ++i;
                while (i < sql.size() && std::isdigit(static_cast<unsigned char>(sql[i]))) {
                    ++i;
                }
            }
            TemplateLiteral literal;
            literal.raw.assign(sql.data() + begin, i - begin);
            literal.display = literal.raw;
            result.literals.push_back(std::move(literal));
            append_normalized_token(result.key, "?", true);
            continue;
        }
        if (sql[i] == '\'') {
            size_t begin = i;
            ++i;
            while (i < sql.size() && sql[i] != '\'') {
                ++i;
            }
            if (i >= sql.size()) {
                return result;
            }
            TemplateLiteral literal;
            literal.is_string = true;
            literal.raw.assign(sql.data() + begin + 1, i - begin - 1);
            literal.display.assign(sql.data() + begin, i - begin + 1);
            result.literals.push_back(std::move(literal));
            ++i;
            append_normalized_token(result.key, "?", true);
            continue;
        }
        if (i + 1 < sql.size()) {
            std::string_view two(sql.data() + i, 2);
            if (two == ">=" || two == "<=" || two == "<>" || two == "!=") {
                append_normalized_token(result.key, two, false);
                i += 2;
                continue;
            }
        }
        append_normalized_token(result.key, std::string_view(sql.data() + i, 1), false);
        ++i;
    }
    result.ok = true;
    return result;
}

bool plan_needs_transaction(const std::shared_ptr<Plan> &plan) {
    switch (plan->tag) {
        case T_Transaction_begin:
        case T_Transaction_commit:
        case T_Transaction_abort:
        case T_Transaction_rollback:
        case T_StaticCheckpoint:
        case T_SetKnob:
        case T_SetIsolation:
            return false;
        default:
            return true;
    }
}

bool plan_invalidates_cache(const std::shared_ptr<Plan> &plan) {
    switch (plan->tag) {
        case T_CreateTable:
        case T_DropTable:
        case T_CreateIndex:
        case T_DropIndex:
        case T_Load:
        case T_SetKnob:
        case T_StaticCheckpoint:
            return true;
        default:
            return false;
    }
}

bool is_plan_cacheable(const std::shared_ptr<Plan> &plan) {
    switch (plan->tag) {
        case T_Insert:
        case T_Update:
        case T_Delete:
        case T_select:
        case T_explain:
        case T_Transaction_begin:
        case T_Transaction_commit:
        case T_Transaction_abort:
        case T_Transaction_rollback:
            return true;
        default:
            return false;
    }
}

uint64_t current_plan_cache_version() {
    return plan_cache_version.load(std::memory_order_acquire);
}

void invalidate_plan_cache() {
    std::lock_guard<std::mutex> lock(plan_cache_latch);
    plan_cache.clear();
    template_plan_cache.clear();
    plan_cache_version.fetch_add(1, std::memory_order_acq_rel);
}

std::shared_ptr<Plan> lookup_cached_plan(const std::string &sql) {
    uint64_t version = current_plan_cache_version();
    std::lock_guard<std::mutex> lock(plan_cache_latch);
    auto it = plan_cache.find(sql);
    if (it == plan_cache.end()) {
        return nullptr;
    }
    if (it->second.version != version) {
        plan_cache.erase(it);
        return nullptr;
    }
    return it->second.plan;
}

void store_cached_plan(const std::string &sql, const std::shared_ptr<Plan> &plan, uint64_t planned_version) {
    if (!is_plan_cacheable(plan) || current_plan_cache_version() != planned_version) {
        return;
    }
    std::lock_guard<std::mutex> lock(plan_cache_latch);
    if (plan_cache_version.load(std::memory_order_acquire) != planned_version) {
        return;
    }
    if (plan_cache.size() >= PLAN_CACHE_MAX_ENTRIES && plan_cache.find(sql) == plan_cache.end()) {
        plan_cache.erase(plan_cache.begin());
    }
    plan_cache[sql] = CachedPlan{planned_version, plan};
}

void assign_expr_params(const std::shared_ptr<ast::Expr> &expr, int &next_param);

void assign_condition_params(const std::shared_ptr<ast::BinaryExpr> &cond, int &next_param) {
    if (cond == nullptr) {
        return;
    }
    assign_expr_params(cond->rhs, next_param);
}

void assign_having_params(const std::shared_ptr<ast::HavingExpr> &cond, int &next_param) {
    if (cond == nullptr) {
        return;
    }
    assign_expr_params(cond->lhs, next_param);
    assign_expr_params(cond->rhs, next_param);
}

void assign_expr_params(const std::shared_ptr<ast::Expr> &expr, int &next_param) {
    if (expr == nullptr) {
        return;
    }
    if (auto value = std::dynamic_pointer_cast<ast::Value>(expr)) {
        value->param_id = next_param++;
    } else if (auto arith = std::dynamic_pointer_cast<ast::ArithmeticExpr>(expr)) {
        assign_expr_params(arith->rhs, next_param);
    }
}

void assign_select_params(const std::shared_ptr<ast::SelectStmt> &select, int &next_param) {
    if (select == nullptr) {
        return;
    }
    for (auto &cond : select->conds) {
        assign_condition_params(cond, next_param);
    }
    for (auto &cond : select->having_conds) {
        assign_having_params(cond, next_param);
    }
    for (auto &ref : select->from_refs) {
        if (ref != nullptr && ref->is_derived) {
            if (auto derived_select = std::dynamic_pointer_cast<ast::SelectStmt>(ref->derived_query)) {
                assign_select_params(derived_select, next_param);
            } else if (auto derived_union = std::dynamic_pointer_cast<ast::UnionStmt>(ref->derived_query)) {
                for (auto &branch : derived_union->branches) {
                    if (auto branch_select = std::dynamic_pointer_cast<ast::SelectStmt>(branch)) {
                        assign_select_params(branch_select, next_param);
                    }
                }
            }
        }
    }
}

int assign_template_param_ids(const std::shared_ptr<ast::TreeNode> &root) {
    int next_param = 0;
    auto assign_root = [&](const std::shared_ptr<ast::TreeNode> &node, auto &&assign_root_ref) -> void {
        if (auto insert = std::dynamic_pointer_cast<ast::InsertStmt>(node)) {
            for (auto &value : insert->vals) {
                if (value != nullptr) {
                    value->param_id = next_param++;
                }
            }
        } else if (auto update = std::dynamic_pointer_cast<ast::UpdateStmt>(node)) {
            for (auto &set_clause : update->set_clauses) {
                if (set_clause != nullptr) {
                    assign_expr_params(set_clause->rhs, next_param);
                }
            }
            for (auto &cond : update->conds) {
                assign_condition_params(cond, next_param);
            }
        } else if (auto del = std::dynamic_pointer_cast<ast::DeleteStmt>(node)) {
            for (auto &cond : del->conds) {
                assign_condition_params(cond, next_param);
            }
        } else if (auto select = std::dynamic_pointer_cast<ast::SelectStmt>(node)) {
            assign_select_params(select, next_param);
        } else if (auto union_stmt = std::dynamic_pointer_cast<ast::UnionStmt>(node)) {
            for (auto &branch : union_stmt->branches) {
                assign_root_ref(branch, assign_root_ref);
            }
        }
    };
    assign_root(root, assign_root);
    return next_param;
}

void merge_value_spec(const Value &value, std::vector<TemplateValueSpec> &specs) {
    if (value.param_id < 0) {
        return;
    }
    if (static_cast<size_t>(value.param_id) >= specs.size()) {
        specs.resize(value.param_id + 1);
    }
    auto &spec = specs[value.param_id];
    int raw_len = value.raw != nullptr ? value.raw->size : 0;
    if (!spec.initialized) {
        spec.type = value.type;
        spec.raw_len = raw_len;
        spec.initialized = true;
        return;
    }
    if (spec.type != value.type) {
        spec.initialized = false;
        return;
    }
    if (spec.raw_len == 0) {
        spec.raw_len = raw_len;
    } else if (raw_len != 0 && spec.raw_len != raw_len) {
        spec.initialized = false;
    }
}

void collect_condition_specs(const std::vector<Condition> &conds, std::vector<TemplateValueSpec> &specs) {
    for (auto &cond : conds) {
        if (cond.is_rhs_val) {
            merge_value_spec(cond.rhs_val, specs);
        }
    }
}

void collect_having_specs(const std::vector<AggHavingCond> &conds, std::vector<TemplateValueSpec> &specs) {
    auto collect_term = [&](const AggTerm &term) {
        if (term.kind == AGG_TERM_VALUE) {
            merge_value_spec(term.val, specs);
        }
    };
    for (auto &cond : conds) {
        collect_term(cond.lhs);
        collect_term(cond.rhs);
    }
}

void collect_plan_specs(const std::shared_ptr<Plan> &plan, std::vector<TemplateValueSpec> &specs) {
    if (plan == nullptr) {
        return;
    }
    if (auto x = std::dynamic_pointer_cast<DMLPlan>(plan)) {
        for (auto &value : x->values_) {
            merge_value_spec(value, specs);
        }
        collect_condition_specs(x->conds_, specs);
        for (auto &set_clause : x->set_clauses_) {
            merge_value_spec(set_clause.rhs, specs);
        }
        collect_plan_specs(x->subplan_, specs);
    } else if (auto x = std::dynamic_pointer_cast<ScanPlan>(plan)) {
        collect_condition_specs(x->conds_, specs);
    } else if (auto x = std::dynamic_pointer_cast<JoinPlan>(plan)) {
        collect_plan_specs(x->left_, specs);
        collect_plan_specs(x->right_, specs);
        collect_condition_specs(x->conds_, specs);
    } else if (auto x = std::dynamic_pointer_cast<ProjectionPlan>(plan)) {
        collect_plan_specs(x->subplan_, specs);
    } else if (auto x = std::dynamic_pointer_cast<FilterPlan>(plan)) {
        collect_plan_specs(x->subplan_, specs);
        collect_condition_specs(x->conds_, specs);
    } else if (auto x = std::dynamic_pointer_cast<SortPlan>(plan)) {
        collect_plan_specs(x->subplan_, specs);
    } else if (auto x = std::dynamic_pointer_cast<RenamePlan>(plan)) {
        collect_plan_specs(x->subplan_, specs);
    } else if (auto x = std::dynamic_pointer_cast<UnionPlan>(plan)) {
        for (auto &child : x->children_) {
            collect_plan_specs(child, specs);
        }
    } else if (auto x = std::dynamic_pointer_cast<AggregatePlan>(plan)) {
        collect_plan_specs(x->subplan_, specs);
        collect_having_specs(x->having_conds_, specs);
    } else if (auto x = std::dynamic_pointer_cast<LimitPlan>(plan)) {
        collect_plan_specs(x->subplan_, specs);
    }
}

std::shared_ptr<Plan> clone_plan(const std::shared_ptr<Plan> &plan) {
    if (plan == nullptr) {
        return nullptr;
    }
    if (auto x = std::dynamic_pointer_cast<DMLPlan>(plan)) {
        return std::make_shared<DMLPlan>(x->tag, clone_plan(x->subplan_), x->tab_name_,
                                         x->values_, x->conds_, x->set_clauses_);
    }
    if (auto x = std::dynamic_pointer_cast<ScanPlan>(plan)) {
        return std::make_shared<ScanPlan>(*x);
    }
    if (auto x = std::dynamic_pointer_cast<JoinPlan>(plan)) {
        return std::make_shared<JoinPlan>(x->tag, clone_plan(x->left_), clone_plan(x->right_), x->conds_);
    }
    if (auto x = std::dynamic_pointer_cast<ProjectionPlan>(plan)) {
        return std::make_shared<ProjectionPlan>(x->tag, clone_plan(x->subplan_), x->sel_cols_, x->is_star_);
    }
    if (auto x = std::dynamic_pointer_cast<FilterPlan>(plan)) {
        return std::make_shared<FilterPlan>(clone_plan(x->subplan_), x->conds_);
    }
    if (auto x = std::dynamic_pointer_cast<SortPlan>(plan)) {
        return std::make_shared<SortPlan>(x->tag, clone_plan(x->subplan_), x->order_bys_);
    }
    if (auto x = std::dynamic_pointer_cast<RenamePlan>(plan)) {
        return std::make_shared<RenamePlan>(clone_plan(x->subplan_), x->cols_);
    }
    if (auto x = std::dynamic_pointer_cast<UnionPlan>(plan)) {
        std::vector<std::shared_ptr<Plan>> children;
        children.reserve(x->children_.size());
        for (auto &child : x->children_) {
            children.push_back(clone_plan(child));
        }
        return std::make_shared<UnionPlan>(std::move(children), x->output_cols_);
    }
    if (auto x = std::dynamic_pointer_cast<AggregatePlan>(plan)) {
        return std::make_shared<AggregatePlan>(clone_plan(x->subplan_), x->select_terms_, x->agg_calls_,
                                               x->group_cols_, x->having_conds_);
    }
    if (auto x = std::dynamic_pointer_cast<LimitPlan>(plan)) {
        return std::make_shared<LimitPlan>(clone_plan(x->subplan_), x->limit_);
    }
    return nullptr;
}

bool parse_integer_literal(const std::string &raw, int &out) {
    errno = 0;
    char *end = nullptr;
    long value = std::strtol(raw.c_str(), &end, 10);
    if (errno != 0 || end == raw.c_str() || *end != '\0' ||
        value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
        return false;
    }
    out = static_cast<int>(value);
    return true;
}

bool parse_float_literal(const std::string &raw, float &out) {
    errno = 0;
    char *end = nullptr;
    float value = std::strtof(raw.c_str(), &end);
    if (errno != 0 || end == raw.c_str() || *end != '\0') {
        return false;
    }
    out = value;
    return true;
}

bool make_bound_value(const TemplateLiteral &literal, const TemplateValueSpec &spec, Value &out) {
    out.param_id = -1;
    out.raw = nullptr;
    if (spec.type == TYPE_STRING) {
        if (!literal.is_string) {
            return false;
        }
        out.set_str(literal.raw);
    } else if (spec.type == TYPE_INT) {
        if (literal.is_string) {
            return false;
        }
        int int_value = 0;
        if (!parse_integer_literal(literal.raw, int_value)) {
            float float_value = 0;
            if (!parse_float_literal(literal.raw, float_value)) {
                return false;
            }
            int_value = static_cast<int>(float_value);
        }
        out.set_int(int_value);
    } else if (spec.type == TYPE_FLOAT) {
        if (literal.is_string) {
            return false;
        }
        float float_value = 0;
        if (!parse_float_literal(literal.raw, float_value)) {
            return false;
        }
        out.set_float(float_value);
    } else {
        return false;
    }
    if (spec.raw_len > 0) {
        out.init_raw(spec.raw_len);
    }
    return true;
}

bool bind_value(Value &value, const std::vector<TemplateValueSpec> &specs,
                const std::vector<TemplateLiteral> &literals) {
    if (value.param_id < 0) {
        return true;
    }
    size_t idx = static_cast<size_t>(value.param_id);
    if (idx >= specs.size() || idx >= literals.size() || !specs[idx].initialized) {
        return false;
    }
    Value bound;
    if (!make_bound_value(literals[idx], specs[idx], bound)) {
        return false;
    }
    bound.param_id = value.param_id;
    value = std::move(bound);
    return true;
}

bool bind_conditions(std::vector<Condition> &conds, const std::vector<TemplateValueSpec> &specs,
                     const std::vector<TemplateLiteral> &literals) {
    for (auto &cond : conds) {
        if (cond.is_rhs_val && cond.rhs_val.param_id >= 0) {
            size_t idx = static_cast<size_t>(cond.rhs_val.param_id);
            if (idx >= literals.size() || !bind_value(cond.rhs_val, specs, literals)) {
                return false;
            }
            cond.rhs_raw = literals[idx].display;
        }
    }
    return true;
}

bool bind_having(std::vector<AggHavingCond> &conds, const std::vector<TemplateValueSpec> &specs,
                 const std::vector<TemplateLiteral> &literals) {
    auto bind_term = [&](AggTerm &term) {
        return term.kind != AGG_TERM_VALUE || bind_value(term.val, specs, literals);
    };
    for (auto &cond : conds) {
        if (!bind_term(cond.lhs) || !bind_term(cond.rhs)) {
            return false;
        }
    }
    return true;
}

bool bind_plan_params(const std::shared_ptr<Plan> &plan, const std::vector<TemplateValueSpec> &specs,
                      const std::vector<TemplateLiteral> &literals) {
    if (plan == nullptr) {
        return true;
    }
    if (auto x = std::dynamic_pointer_cast<DMLPlan>(plan)) {
        for (auto &value : x->values_) {
            if (!bind_value(value, specs, literals)) {
                return false;
            }
        }
        if (!bind_conditions(x->conds_, specs, literals)) {
            return false;
        }
        for (auto &set_clause : x->set_clauses_) {
            if (!bind_value(set_clause.rhs, specs, literals)) {
                return false;
            }
        }
        return bind_plan_params(x->subplan_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<ScanPlan>(plan)) {
        return bind_conditions(x->conds_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<JoinPlan>(plan)) {
        return bind_plan_params(x->left_, specs, literals) &&
               bind_plan_params(x->right_, specs, literals) &&
               bind_conditions(x->conds_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<ProjectionPlan>(plan)) {
        return bind_plan_params(x->subplan_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<FilterPlan>(plan)) {
        return bind_plan_params(x->subplan_, specs, literals) &&
               bind_conditions(x->conds_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<SortPlan>(plan)) {
        return bind_plan_params(x->subplan_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<RenamePlan>(plan)) {
        return bind_plan_params(x->subplan_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<UnionPlan>(plan)) {
        for (auto &child : x->children_) {
            if (!bind_plan_params(child, specs, literals)) {
                return false;
            }
        }
        return true;
    }
    if (auto x = std::dynamic_pointer_cast<AggregatePlan>(plan)) {
        return bind_plan_params(x->subplan_, specs, literals) &&
               bind_having(x->having_conds_, specs, literals);
    }
    if (auto x = std::dynamic_pointer_cast<LimitPlan>(plan)) {
        return bind_plan_params(x->subplan_, specs, literals);
    }
    return false;
}

std::shared_ptr<Plan> lookup_template_plan(const SqlTemplate &sql_template) {
    if (!sql_template.ok || sql_template.literals.empty()) {
        return nullptr;
    }
    CachedTemplatePlan cached;
    uint64_t version = current_plan_cache_version();
    {
        std::lock_guard<std::mutex> lock(plan_cache_latch);
        auto it = template_plan_cache.find(sql_template.key);
        if (it == template_plan_cache.end()) {
            return nullptr;
        }
        if (it->second.version != version) {
            template_plan_cache.erase(it);
            return nullptr;
        }
        if (it->second.specs.size() != sql_template.literals.size()) {
            return nullptr;
        }
        cached = it->second;
    }
    auto plan = clone_plan(cached.plan);
    if (plan == nullptr || !bind_plan_params(plan, cached.specs, sql_template.literals)) {
        return nullptr;
    }
    return plan;
}

void store_template_plan(const SqlTemplate &sql_template, const std::shared_ptr<Plan> &plan,
                         int param_count, uint64_t planned_version) {
    if (!sql_template.ok || sql_template.literals.empty() || param_count <= 0 ||
        static_cast<size_t>(param_count) != sql_template.literals.size() ||
        !is_plan_cacheable(plan) || current_plan_cache_version() != planned_version) {
        return;
    }
    std::vector<TemplateValueSpec> specs(param_count);
    collect_plan_specs(plan, specs);
    if (specs.size() != static_cast<size_t>(param_count)) {
        return;
    }
    for (auto &spec : specs) {
        if (!spec.initialized) {
            return;
        }
    }
    std::lock_guard<std::mutex> lock(plan_cache_latch);
    if (plan_cache_version.load(std::memory_order_acquire) != planned_version) {
        return;
    }
    if (template_plan_cache.size() >= PLAN_CACHE_MAX_ENTRIES &&
        template_plan_cache.find(sql_template.key) == template_plan_cache.end()) {
        template_plan_cache.erase(template_plan_cache.begin());
    }
    template_plan_cache[sql_template.key] = CachedTemplatePlan{planned_version, plan, std::move(specs)};
}

}  // namespace

static void EnsureStatementTransaction(txn_id_t *txn_id, Context *context,
                                       IsolationLevel session_isolation_level) {
    context->txn_ = txn_manager->get_transaction(*txn_id);
    if (context->txn_ == nullptr || context->txn_->get_state() == TransactionState::COMMITTED ||
        context->txn_->get_state() == TransactionState::ABORTED) {
        context->txn_ = txn_manager->begin(nullptr, context->log_mgr_, session_isolation_level, false);
        *txn_id = context->txn_->get_transaction_id();
        context->txn_->set_txn_mode(false);
    }
}

static void AbortActiveTransaction(txn_id_t *txn_id, Context *context) {
    if (context->txn_ != nullptr &&
        context->txn_->get_state() != TransactionState::COMMITTED &&
        context->txn_->get_state() != TransactionState::ABORTED) {
        txn_manager->abort(context->txn_, context->log_mgr_);
    }
    txn_manager->release_transaction(context->txn_);
    *txn_id = INVALID_TXN_ID;
    context->txn_ = nullptr;
}

static void AbortTransactionById(txn_id_t *txn_id) {
    if (txn_id == nullptr || *txn_id == INVALID_TXN_ID) {
        return;
    }
    Transaction *txn = txn_manager->get_transaction(*txn_id);
    if (txn != nullptr &&
        txn->get_state() != TransactionState::COMMITTED &&
        txn->get_state() != TransactionState::ABORTED) {
        txn_manager->abort(txn, log_manager.get());
    }
    txn_manager->release_transaction(txn);
    *txn_id = INVALID_TXN_ID;
}

static std::string_view trim_command_view(const char *data, size_t len) {
    size_t begin = 0;
    while (begin < len && std::isspace(static_cast<unsigned char>(data[begin]))) {
        ++begin;
    }
    while (len > begin &&
           (std::isspace(static_cast<unsigned char>(data[len - 1])) || data[len - 1] == '\0')) {
        --len;
    }
    return std::string_view(data + begin, len - begin);
}

static bool iequals(std::string_view lhs, const char *rhs) {
    const size_t rhs_len = std::strlen(rhs);
    if (lhs.size() != rhs_len) {
        return false;
    }
    for (size_t i = 0; i < rhs_len; ++i) {
        if (std::tolower(static_cast<unsigned char>(lhs[i])) !=
            std::tolower(static_cast<unsigned char>(rhs[i]))) {
            return false;
        }
    }
    return true;
}

static bool next_token(std::string_view s, size_t *pos, std::string_view *token) {
    while (*pos < s.size() && std::isspace(static_cast<unsigned char>(s[*pos]))) {
        ++(*pos);
    }
    if (*pos >= s.size()) {
        return false;
    }
    const size_t begin = *pos;
    while (*pos < s.size() && !std::isspace(static_cast<unsigned char>(s[*pos]))) {
        ++(*pos);
    }
    *token = s.substr(begin, *pos - begin);
    return true;
}

static bool parse_load_command(std::string_view cmd, std::string &file_name, std::string &tab_name) {
    if (!cmd.empty() && cmd.back() == ';') {
        cmd.remove_suffix(1);
    }

    size_t pos = 0;
    std::string_view load_kw;
    std::string_view file_token;
    std::string_view into_kw;
    std::string_view table_token;
    std::string_view extra;
    if (!next_token(cmd, &pos, &load_kw) ||
        !next_token(cmd, &pos, &file_token) ||
        !next_token(cmd, &pos, &into_kw) ||
        !next_token(cmd, &pos, &table_token) ||
        next_token(cmd, &pos, &extra)) {
        return false;
    }
    if (!iequals(load_kw, "load") || !iequals(into_kw, "into")) {
        return false;
    }
    file_name.assign(file_token.data(), file_token.size());
    tab_name.assign(table_token.data(), table_token.size());
    return true;
}

void *client_handler(void *sock_fd) {
    int fd = *((int *)sock_fd);
    delete (int *)sock_fd;

    int i_recvBytes;
    // 接收客户端发送的请求
    char data_recv[BUFFER_LENGTH];
    // 需要返回给客户端的结果
    char *data_send = new char[BUFFER_LENGTH];
    // 需要返回给客户端的结果的长度
    int offset = 0;
    // 记录客户端当前正在执行的事务ID
    txn_id_t txn_id = INVALID_TXN_ID;
    IsolationLevel session_isolation_level = IsolationLevel::READ_COMMITTED;
    bool output_file_enabled = true;

    while (true) {
        memset(data_recv, 0, BUFFER_LENGTH);

        i_recvBytes = read(fd, data_recv, BUFFER_LENGTH);

        if (i_recvBytes == 0) {
            break;
        }
        if (i_recvBytes == -1) {
            break;
        }

        const std::string_view raw_cmd = trim_command_view(data_recv, i_recvBytes);
        if (iequals(raw_cmd, "exit")) {
            break;
        }
        if (iequals(raw_cmd, "crash")) {
            exit(1);
        }

        memset(data_send, '\0', BUFFER_LENGTH);
        offset = 0;

        // 开启事务，初始化系统所需的上下文信息（包括事务对象指针、锁管理器指针、日志管理器指针、存放结果的buffer、记录结果长度的变量）
        Context context(lock_manager.get(), log_manager.get(), nullptr, data_send, &offset,
                        txn_manager.get(), &session_isolation_level,
                        &isolation_output_format, &output_file_enabled);

        if (iequals(raw_cmd, "set output_file off")) {
            output_file_enabled = false;
            if (write(fd, data_send, offset + 1) == -1) {
                break;
            }
            continue;
        }
        std::string load_file;
        std::string load_table;
        if (parse_load_command(raw_cmd, load_file, load_table)) {
            try {
                EnsureStatementTransaction(&txn_id, &context, session_isolation_level);
                sm_manager->load_table(load_file, load_table, &context);
                invalidate_plan_cache();
            } catch (TransactionAbortException &e) {
                std::string str = "abort\n";
                memcpy(data_send, str.c_str(), str.length());
                data_send[str.length()] = '\0';
                offset = str.length();
                AbortActiveTransaction(&txn_id, &context);
                if (output_file_enabled) {
                    std::fstream outfile;
                    outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                    outfile << str;
                    outfile.close();
                }
            } catch (RMDBError &e) {
                AbortActiveTransaction(&txn_id, &context);
                memcpy(data_send, e.what(), e.get_msg_len());
                data_send[e.get_msg_len()] = '\n';
                data_send[e.get_msg_len() + 1] = '\0';
                offset = e.get_msg_len() + 1;
                if (output_file_enabled) {
                    std::fstream outfile;
                    outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                    outfile << "failure\n";
                    outfile.close();
                }
            }
            if(context.txn_ != nullptr && context.txn_->get_txn_mode() == false &&
               context.txn_->get_state() != TransactionState::COMMITTED &&
               context.txn_->get_state() != TransactionState::ABORTED)
            {
                txn_manager->commit(context.txn_, context.log_mgr_);
                txn_manager->release_transaction(context.txn_);
                context.txn_ = nullptr;
                txn_id = INVALID_TXN_ID;
            }
            if (write(fd, data_send, offset + 1) == -1) {
                break;
            }
            continue;
        }

        auto handle_transaction_abort = [&](TransactionAbortException &) {
            std::string str = "abort\n";
            memcpy(data_send, str.c_str(), str.length());
            data_send[str.length()] = '\0';
            offset = str.length();

            AbortActiveTransaction(&txn_id, &context);

            if (output_file_enabled) {
                std::fstream outfile;
                outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                outfile << str;
                outfile.close();
            }
        };

        auto handle_rmdb_error = [&](RMDBError &e) {
            AbortActiveTransaction(&txn_id, &context);

            memcpy(data_send, e.what(), e.get_msg_len());
            data_send[e.get_msg_len()] = '\n';
            data_send[e.get_msg_len() + 1] = '\0';
            offset = e.get_msg_len() + 1;

            if (output_file_enabled) {
                std::fstream outfile;
                outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                outfile << "failure\n";
                outfile.close();
            }
        };

        auto execute_plan = [&](const std::shared_ptr<Plan> &plan) {
            if (plan_needs_transaction(plan)) {
                EnsureStatementTransaction(&txn_id, &context, session_isolation_level);
            }
            std::shared_ptr<PortalStmt> portalStmt = portal->start(plan, &context);
            portal->run(portalStmt, ql_manager.get(), &txn_id, &context);
            portal->drop();
            if (plan->tag == T_SetIsolation) {
                isolation_output_format.store(true);
            }
            if (plan_invalidates_cache(plan)) {
                invalidate_plan_cache();
            }
        };

        std::string sql_key(raw_cmd.data(), raw_cmd.size());
        if (auto cached_plan = lookup_cached_plan(sql_key)) {
            try {
                execute_plan(cached_plan);
            } catch (TransactionAbortException &e) {
                handle_transaction_abort(e);
            } catch (RMDBError &e) {
                handle_rmdb_error(e);
            }
            if(context.txn_ != nullptr && context.txn_->get_txn_mode() == false &&
               context.txn_->get_state() != TransactionState::COMMITTED &&
               context.txn_->get_state() != TransactionState::ABORTED)
            {
                txn_manager->commit(context.txn_, context.log_mgr_);
                txn_manager->release_transaction(context.txn_);
                context.txn_ = nullptr;
                txn_id = INVALID_TXN_ID;
            }
            if (write(fd, data_send, offset + 1) == -1) {
                break;
            }
            continue;
        }
        SqlTemplate sql_template = build_sql_template(raw_cmd);
        if (auto cached_template_plan = lookup_template_plan(sql_template)) {
            try {
                execute_plan(cached_template_plan);
            } catch (TransactionAbortException &e) {
                handle_transaction_abort(e);
            } catch (RMDBError &e) {
                handle_rmdb_error(e);
            }
            if(context.txn_ != nullptr && context.txn_->get_txn_mode() == false &&
               context.txn_->get_state() != TransactionState::COMMITTED &&
               context.txn_->get_state() != TransactionState::ABORTED)
            {
                txn_manager->commit(context.txn_, context.log_mgr_);
                txn_manager->release_transaction(context.txn_);
                context.txn_ = nullptr;
                txn_id = INVALID_TXN_ID;
            }
            if (write(fd, data_send, offset + 1) == -1) {
                break;
            }
            continue;
        }

        // 用于判断是否已经调用了yy_delete_buffer来删除buf
        bool finish_analyze = false;
        uint64_t planned_version = current_plan_cache_version();
        pthread_mutex_lock(buffer_mutex);
        YY_BUFFER_STATE buf = yy_scan_string(data_recv);
        if (yyparse() == 0) {
            if (ast::parse_tree != nullptr) {
                try {
                    int template_param_count = sql_template.ok ? assign_template_param_ids(ast::parse_tree) : 0;
                    // analyze and rewrite
                    std::shared_ptr<Query> query = analyze->do_analyze(ast::parse_tree);
                    yy_delete_buffer(buf);
                    finish_analyze = true;
                    pthread_mutex_unlock(buffer_mutex);
                    // 优化器
                    std::shared_ptr<Plan> plan = optimizer->plan_query(query, &context);
                    execute_plan(plan);
                    store_cached_plan(sql_key, plan, planned_version);
                    store_template_plan(sql_template, plan, template_param_count, planned_version);
                } catch (TransactionAbortException &e) {
                    handle_transaction_abort(e);
                } catch (RMDBError &e) {
                    handle_rmdb_error(e);
                }
            }
        } else {
            std::string str = "failure\n";
            memcpy(data_send, str.c_str(), str.length());
            data_send[str.length()] = '\0';
            offset = str.length();

            if (output_file_enabled) {
                std::fstream outfile;
                outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                outfile << "failure\n";
                outfile.close();
            }
        }
        if(finish_analyze == false) {
            yy_delete_buffer(buf);
            pthread_mutex_unlock(buffer_mutex);
        }
        // future TODO: 格式化 sql_handler.result, 传给客户端
        // send result with fixed format, use protobuf in the future
        // 如果是单挑语句，需要按照一个完整的事务来执行，所以执行完当前语句后，自动提交事务
        if(context.txn_ != nullptr && context.txn_->get_txn_mode() == false &&
           context.txn_->get_state() != TransactionState::COMMITTED &&
           context.txn_->get_state() != TransactionState::ABORTED)
        {
            txn_manager->commit(context.txn_, context.log_mgr_);
            txn_manager->release_transaction(context.txn_);
            context.txn_ = nullptr;
            txn_id = INVALID_TXN_ID;
        }
        if (write(fd, data_send, offset + 1) == -1) {
            break;
        }
    }

    // Clear
    AbortTransactionById(&txn_id);
    delete[] data_send;
    close(fd);           // close a file descriptor.
    pthread_exit(NULL);  // terminate calling thread!
}

void start_server() {
    // init mutex
    buffer_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(buffer_mutex, nullptr);

    int sockfd_server;
    int fd_temp;
    struct sockaddr_in s_addr_in {};

    // 初始化连接
    sockfd_server = socket(AF_INET, SOCK_STREAM, 0);  // ipv4,TCP
    assert(sockfd_server != -1);
    int val = 1;
    setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // before bind(), set the attr of structure sockaddr.
    memset(&s_addr_in, 0, sizeof(s_addr_in));
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr_in.sin_port = htons(SOCK_PORT);
    fd_temp = bind(sockfd_server, (struct sockaddr *)(&s_addr_in), sizeof(s_addr_in));
    if (fd_temp == -1) {
        std::cout << "Bind error!" << std::endl;
        exit(1);
    }

    fd_temp = listen(sockfd_server, MAX_CONN_LIMIT);
    if (fd_temp == -1) {
        std::cout << "Listen error!" << std::endl;
        exit(1);
    }

    while (!should_exit) {
        std::cout << "Waiting for new connection..." << std::endl;
        pthread_t thread_id;
        struct sockaddr_in s_addr_client {};
        int client_length = sizeof(s_addr_client);

        if (setjmp(jmpbuf)) {
            std::cout << "Break from Server Listen Loop\n";
            break;
        }

        // Block here. Until server accepts a new connection.
        int sockfd = accept(sockfd_server, (struct sockaddr *)(&s_addr_client), (socklen_t *)(&client_length));
        if (sockfd == -1) {
            std::cout << "Accept error!" << std::endl;
            continue;  // ignore current socket ,continue while loop.
        }

        // 和客户端建立连接，并开启一个线程负责处理客户端请求
        if (pthread_create(&thread_id, nullptr, &client_handler, (void *)(new int(sockfd))) != 0) {
            std::cout << "Create thread fail!" << std::endl;
            break;  // break while loop
        }

    }

    // Clear
    std::cout << " Try to close all client-connection.\n";
    int ret = shutdown(sockfd_server, SHUT_WR);  // shut down the all or part of a full-duplex connection.
    if(ret == -1) { printf("%s\n", strerror(errno)); }
//    assert(ret != -1);
    sm_manager->close_db();
    std::cout << " DB has been closed.\n";
    std::cout << "Server shuts down." << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        // 需要指定数据库名称
        std::cerr << "Usage: " << argv[0] << " <database>" << std::endl;
        exit(1);
    }

    signal(SIGINT, sigint_handler);
    try {
        std::cout << "\n"
                     "  _____  __  __ _____  ____  \n"
                     " |  __ \\|  \\/  |  __ \\|  _ \\ \n"
                     " | |__) | \\  / | |  | | |_) |\n"
                     " |  _  /| |\\/| | |  | |  _ < \n"
                     " | | \\ \\| |  | | |__| | |_) |\n"
                     " |_|  \\_\\_|  |_|_____/|____/ \n"
                     "\n"
                     "Welcome to RMDB!\n"
                     "Type 'help;' for help.\n"
                     "\n";
        // Database name is passed by args
        std::string db_name = argv[1];
        if (!sm_manager->is_dir(db_name)) {
            // Database not found, create a new one
            sm_manager->create_db(db_name);
        }
        // Open database
        sm_manager->open_db(db_name);
        disk_manager->SetLogFd(disk_manager->open_file(db_name + "/" + LOG_FILE_NAME));

        // recovery database
        recovery->analyze();
        recovery->redo();
        recovery->undo();

        // 开启服务端，开始接受客户端连接
        start_server();
    } catch (RMDBError &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
