/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "execution_manager.h"

#include "executor_delete.h"
#include "executor_index_scan.h"
#include "executor_insert.h"
#include "executor_nestedloop_join.h"
#include "executor_projection.h"
#include "executor_seq_scan.h"
#include "executor_update.h"
#include "index/ix.h"
#include "record_printer.h"

#include <iomanip>
#include <sstream>

const char *help_info = "Supported SQL syntax:\n"
                   "  command ;\n"
                   "command:\n"
                   "  CREATE TABLE table_name (column_name type [, column_name type ...])\n"
                   "  DROP TABLE table_name\n"
                   "  CREATE INDEX table_name (column_name)\n"
                   "  DROP INDEX table_name (column_name)\n"
                   "  INSERT INTO table_name VALUES (value [, value ...])\n"
                   "  DELETE FROM table_name [WHERE where_clause]\n"
                   "  UPDATE table_name SET column_name = value [, column_name = value ...] [WHERE where_clause]\n"
                   "  SELECT selector FROM table_name [WHERE where_clause]\n"
                   "type:\n"
                   "  {INT | FLOAT | CHAR(n)}\n"
                   "where_clause:\n"
                   "  condition [AND condition ...]\n"
                   "condition:\n"
                   "  column op {column | value}\n"
                   "column:\n"
                   "  [table_name.]column_name\n"
                   "op:\n"
                   "  {= | <> | < | > | <= | >=}\n"
                   "selector:\n"
                   "  {* | column [, column ...]}\n";

static std::string format_select_output(const std::vector<std::string> &captions,
                                        const std::vector<std::vector<std::string>> &rows) {
    constexpr size_t COL_WIDTH = 16;
    std::ostringstream os;
    auto print_separator = [&]() {
        for (size_t i = 0; i < captions.size(); ++i) {
            os << "+" << std::string(COL_WIDTH + 2, '-');
        }
        os << "+\n";
    };
    auto print_record = [&](const std::vector<std::string> &cols) {
        for (auto col : cols) {
            if (col.size() > COL_WIDTH) {
                col = col.substr(0, COL_WIDTH - 3) + "...";
            }
            os << "| " << std::setw(COL_WIDTH) << col << " ";
        }
        os << "|\n";
    };

    print_separator();
    print_record(captions);
    print_separator();
    for (const auto &row : rows) {
        print_record(row);
    }
    print_separator();
    os << "Total record(s): " << rows.size() << "\n";
    return os.str();
}

static std::string format_legacy_output(const std::vector<std::string> &captions,
                                        const std::vector<std::vector<std::string>> &rows) {
    std::ostringstream os;
    auto print_record = [&](const std::vector<std::string> &cols) {
        for (auto &col : cols) {
            os << "| " << col << " ";
        }
        os << "|\n";
    };
    print_record(captions);
    for (auto &row : rows) {
        print_record(row);
    }
    return os.str();
}

// 主要负责执行DDL语句
void QlManager::run_mutli_query(std::shared_ptr<Plan> plan, Context *context){
    if (auto x = std::dynamic_pointer_cast<DDLPlan>(plan)) {
        switch(x->tag) {
            case T_CreateTable:
            {
                sm_manager_->create_table(x->tab_name_, x->cols_, context);
                break;
            }
            case T_DropTable:
            {
                sm_manager_->drop_table(x->tab_name_, context);
                break;
            }
            case T_CreateIndex:
            {
                sm_manager_->create_index(x->tab_name_, x->tab_col_names_, context);
                break;
            }
            case T_DropIndex:
            {
                sm_manager_->drop_index(x->tab_name_, x->tab_col_names_, context);
                break;
            }
            default:
                throw InternalError("Unexpected field type");
                break;
        }
    }
}

// 执行help; show tables; desc table; begin; commit; abort;语句
void QlManager::run_cmd_utility(std::shared_ptr<Plan> plan, txn_id_t *txn_id, Context *context) {
    if (auto x = std::dynamic_pointer_cast<OtherPlan>(plan)) {
        switch(x->tag) {
            case T_Help:
            {
                memcpy(context->data_send_ + *(context->offset_), help_info, strlen(help_info));
                *(context->offset_) = strlen(help_info);
                break;
            }
            case T_ShowTable:
            {
                sm_manager_->show_tables(context);
                break;
            }
            case T_ShowIndex:
            {
                sm_manager_->show_index(x->tab_name_, context);
                break;
            }
            case T_DescTable:
            {
                sm_manager_->desc_table(x->tab_name_, context);
                break;
            }
            case T_Transaction_begin:
            {
                if (context->session_isolation_level_ == nullptr) {
                    throw RMDBError("failure");
                }
                if (context->txn_ != nullptr &&
                    context->txn_->get_state() != TransactionState::COMMITTED &&
                    context->txn_->get_state() != TransactionState::ABORTED) {
                    break;
                }
                context->txn_ = txn_mgr_->begin(nullptr, context->log_mgr_, *context->session_isolation_level_);
                context->txn_->set_txn_mode(true);
                *txn_id = context->txn_->get_transaction_id();
                break;
            }
            case T_Transaction_commit:
            {
                context->txn_ = txn_mgr_->get_transaction(*txn_id);
                if (context->txn_ != nullptr &&
                    context->txn_->get_state() != TransactionState::COMMITTED &&
                    context->txn_->get_state() != TransactionState::ABORTED) {
                    txn_mgr_->commit(context->txn_, context->log_mgr_);
                }
                *txn_id = INVALID_TXN_ID;
                context->txn_ = nullptr;
                break;
            }
            case T_Transaction_rollback:
            {
                context->txn_ = txn_mgr_->get_transaction(*txn_id);
                if (context->txn_ != nullptr &&
                    context->txn_->get_state() != TransactionState::COMMITTED &&
                    context->txn_->get_state() != TransactionState::ABORTED) {
                    txn_mgr_->abort(context->txn_, context->log_mgr_);
                }
                *txn_id = INVALID_TXN_ID;
                context->txn_ = nullptr;
                break;
            }
            case T_Transaction_abort:
            {
                context->txn_ = txn_mgr_->get_transaction(*txn_id);
                if (context->txn_ != nullptr &&
                    context->txn_->get_state() != TransactionState::COMMITTED &&
                    context->txn_->get_state() != TransactionState::ABORTED) {
                    txn_mgr_->abort(context->txn_, context->log_mgr_);
                }
                *txn_id = INVALID_TXN_ID;
                context->txn_ = nullptr;
                break;
            }
            case T_StaticCheckpoint:
            {
                sm_manager_->create_static_checkpoint(context->log_mgr_);
                break;
            }
            default:
                throw InternalError("Unexpected field type");
                break;
        }

    } else if(auto x = std::dynamic_pointer_cast<SetKnobPlan>(plan)) {
        switch (x->set_knob_type_)
        {
        case ast::SetKnobType::EnableNestLoop: {
            planner_->set_enable_nestedloop_join(x->bool_value_);
            break;
        }
        case ast::SetKnobType::EnableSortMerge: {
            planner_->set_enable_sortmerge_join(x->bool_value_);
            break;
        }
        case ast::SetKnobType::OutputFile: {
            if (context->output_file_enabled_ != nullptr) {
                *context->output_file_enabled_ = x->bool_value_;
            }
            break;
        }
        default: {
            throw RMDBError("Not implemented!\n");
            break;
        }
        }
    } else if(auto x = std::dynamic_pointer_cast<SetIsolationPlan>(plan)) {
        if (context->session_isolation_level_ == nullptr) {
            throw RMDBError("failure");
        }
        switch (x->isolation_level_) {
        case ast::IsolationLevelType::SnapshotIsolation:
            *context->session_isolation_level_ = IsolationLevel::SNAPSHOT_ISOLATION;
            break;
        case ast::IsolationLevelType::Serializable:
            *context->session_isolation_level_ = IsolationLevel::SERIALIZABLE;
            break;
        default:
            throw RMDBError("failure");
        }
    } else if (auto x = std::dynamic_pointer_cast<LoadPlan>(plan)) {
        sm_manager_->load_table(x->file_name_, x->tab_name_, context);
    }
}

// 执行select语句，select语句的输出除了需要返回客户端外，还需要写入output.txt文件中
void QlManager::select_from(std::unique_ptr<AbstractExecutor> executorTreeRoot, std::vector<TabCol> sel_cols,
                            Context *context) {
    std::vector<std::string> captions;
    captions.reserve(sel_cols.size());
    for (auto &sel_col : sel_cols) {
        captions.push_back(sel_col.col_name);
    }

    std::vector<std::vector<std::string>> rows;
    for (executorTreeRoot->beginTuple(); !executorTreeRoot->is_end(); executorTreeRoot->nextTuple()) {
        auto Tuple = executorTreeRoot->Next();
        std::vector<std::string> columns;
        for (auto &col : executorTreeRoot->cols()) {
            std::string col_str;
            char *rec_buf = Tuple->data + col.offset;
            if (col.type == TYPE_INT) {
                col_str = std::to_string(*(int *)rec_buf);
            } else if (col.type == TYPE_FLOAT) {
                std::ostringstream os;
                os << std::fixed << std::setprecision(6) << *(float *)rec_buf;
                col_str = os.str();
            } else if (col.type == TYPE_STRING) {
                col_str = std::string((char *)rec_buf, col.len);
                col_str.resize(strlen(col_str.c_str()));
            }
            columns.push_back(col_str);
        }
        rows.push_back(std::move(columns));
    }
    executorTreeRoot->finish();

    // Print header into buffer
    RecordPrinter rec_printer(sel_cols.size());
    rec_printer.print_separator(context);
    rec_printer.print_record(captions, context);
    rec_printer.print_separator(context);
    // Print records
    for (auto &columns : rows) {
        // print record into buffer
        rec_printer.print_record(columns, context);
    }
    // Print footer into buffer
    rec_printer.print_separator(context);
    // Print record count into buffer
    RecordPrinter::print_record_count(rows.size(), context);

    if (context->output_file_enabled_ == nullptr || *context->output_file_enabled_) {
        std::fstream outfile;
        outfile.open(sm_manager_->db_.name() + "/output.txt", std::ios::out | std::ios::app);
        if (context->isolation_output_format_ != nullptr && context->isolation_output_format_->load()) {
            outfile << format_select_output(captions, rows);
        } else {
            outfile << format_legacy_output(captions, rows);
        }
        outfile.close();
    }
}

// 执行DML语句
void QlManager::run_dml(std::unique_ptr<AbstractExecutor> exec){
    exec->Next();
}

// 题目四：执行 EXPLAIN ANALYZE 计划树，统计各节点运行时行数后输出计划树（不输出结果集）
void QlManager::run_explain(std::unique_ptr<AbstractExecutor> executorTreeRoot, Context *context) {
    // 完整执行计划，累计各节点 rows_
    for (executorTreeRoot->beginTuple(); !executorTreeRoot->is_end(); executorTreeRoot->nextTuple()) {
        executorTreeRoot->Next();
    }
    executorTreeRoot->finish();
    // 生成计划树文本
    std::string out;
    executorTreeRoot->explain_print(0, out);

    // 输出到客户端缓冲区
    memcpy(context->data_send_ + *(context->offset_), out.c_str(), out.length());
    *(context->offset_) += out.length();

    // 输出到 output.txt（追加）
    if (context->output_file_enabled_ == nullptr || *context->output_file_enabled_) {
        std::fstream outfile;
        outfile.open(sm_manager_->db_.name() + "/output.txt", std::ios::out | std::ios::app);
        outfile << out;
        outfile.close();
    }
}
