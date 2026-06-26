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
#include <atomic>
#include <algorithm>
#include <cctype>
#include <memory>
#include <sstream>
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
auto ql_manager = std::make_unique<QlManager>(sm_manager.get(), txn_manager.get(), nullptr);
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

static bool is_transaction_control_query(const std::shared_ptr<Query> &query) {
    return std::dynamic_pointer_cast<ast::TxnBegin>(query->parse) != nullptr ||
           std::dynamic_pointer_cast<ast::TxnCommit>(query->parse) != nullptr ||
           std::dynamic_pointer_cast<ast::TxnAbort>(query->parse) != nullptr ||
           std::dynamic_pointer_cast<ast::TxnRollback>(query->parse) != nullptr ||
           std::dynamic_pointer_cast<ast::StaticCheckpoint>(query->parse) != nullptr;
}

static bool is_session_only_query(const std::shared_ptr<Query> &query) {
    return std::dynamic_pointer_cast<ast::SetIsolationStmt>(query->parse) != nullptr ||
           std::dynamic_pointer_cast<ast::SetStmt>(query->parse) != nullptr;
}

static bool query_needs_transaction(const std::shared_ptr<Query> &query) {
    return !is_transaction_control_query(query) && !is_session_only_query(query);
}

static void EnsureStatementTransaction(txn_id_t *txn_id, Context *context,
                                       IsolationLevel session_isolation_level) {
    context->txn_ = txn_manager->get_transaction(*txn_id);
    if (context->txn_ == nullptr || context->txn_->get_state() == TransactionState::COMMITTED ||
        context->txn_->get_state() == TransactionState::ABORTED) {
        context->txn_ = txn_manager->begin(nullptr, context->log_mgr_, session_isolation_level);
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

static std::string trim_command(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
        s.erase(s.begin());
    }
    while (!s.empty() && (std::isspace(static_cast<unsigned char>(s.back())) || s.back() == '\0')) {
        s.pop_back();
    }
    return s;
}

static std::string lower_command(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

static bool parse_load_command(const std::string &cmd, std::string &file_name, std::string &tab_name) {
    std::string s = trim_command(cmd);
    if (!s.empty() && s.back() == ';') {
        s.pop_back();
    }
    std::istringstream iss(s);
    std::string load_kw;
    std::string into_kw;
    if (!(iss >> load_kw >> file_name >> into_kw >> tab_name)) {
        return false;
    }
    std::string extra;
    if (iss >> extra) {
        return false;
    }
    return lower_command(load_kw) == "load" && lower_command(into_kw) == "into";
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

        if (strcmp(data_recv, "exit") == 0) {
            break;
        }
        if (strcmp(data_recv, "crash") == 0) {
            exit(1);
        }

        memset(data_send, '\0', BUFFER_LENGTH);
        offset = 0;

        // 开启事务，初始化系统所需的上下文信息（包括事务对象指针、锁管理器指针、日志管理器指针、存放结果的buffer、记录结果长度的变量）
        auto context = std::make_unique<Context>(lock_manager.get(), log_manager.get(), nullptr, data_send, &offset,
                                                 txn_manager.get(), &session_isolation_level,
                                                 &isolation_output_format, &output_file_enabled);

        std::string raw_cmd = trim_command(data_recv);
        if (lower_command(raw_cmd) == "set output_file off") {
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
                EnsureStatementTransaction(&txn_id, context.get(), session_isolation_level);
                sm_manager->load_table(load_file, load_table, context.get());
            } catch (TransactionAbortException &e) {
                std::string str = "abort\n";
                memcpy(data_send, str.c_str(), str.length());
                data_send[str.length()] = '\0';
                offset = str.length();
                AbortActiveTransaction(&txn_id, context.get());
                if (output_file_enabled) {
                    std::fstream outfile;
                    outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                    outfile << str;
                    outfile.close();
                }
            } catch (RMDBError &e) {
                AbortActiveTransaction(&txn_id, context.get());
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
            if (write(fd, data_send, offset + 1) == -1) {
                break;
            }
            if(context->txn_ != nullptr && context->txn_->get_txn_mode() == false &&
               context->txn_->get_state() != TransactionState::COMMITTED &&
               context->txn_->get_state() != TransactionState::ABORTED)
            {
                txn_manager->commit(context->txn_, context->log_mgr_);
                txn_manager->release_transaction(context->txn_);
                context->txn_ = nullptr;
                txn_id = INVALID_TXN_ID;
            }
            continue;
        }

        // 用于判断是否已经调用了yy_delete_buffer来删除buf
        bool finish_analyze = false;
        pthread_mutex_lock(buffer_mutex);
        YY_BUFFER_STATE buf = yy_scan_string(data_recv);
        if (yyparse() == 0) {
            if (ast::parse_tree != nullptr) {
                try {
                    // analyze and rewrite
                    std::shared_ptr<Query> query = analyze->do_analyze(ast::parse_tree);
                    yy_delete_buffer(buf);
                    finish_analyze = true;
                    pthread_mutex_unlock(buffer_mutex);
                    if (query_needs_transaction(query)) {
                        EnsureStatementTransaction(&txn_id, context.get(), session_isolation_level);
                    }
                    // 优化器
                    std::shared_ptr<Plan> plan = optimizer->plan_query(query, context.get());
                    // portal
                    std::shared_ptr<PortalStmt> portalStmt = portal->start(plan, context.get());
                    portal->run(portalStmt, ql_manager.get(), &txn_id, context.get());
                    portal->drop();
                    if (std::dynamic_pointer_cast<SetIsolationPlan>(plan) != nullptr) {
                        isolation_output_format.store(true);
                    }
                } catch (TransactionAbortException &e) {
                    // 事务需要回滚，需要把abort信息返回给客户端并写入output.txt文件中
                    std::string str = "abort\n";
                    memcpy(data_send, str.c_str(), str.length());
                    data_send[str.length()] = '\0';
                    offset = str.length();

                    // 回滚事务
                    AbortActiveTransaction(&txn_id, context.get());

                    if (output_file_enabled) {
                        std::fstream outfile;
                        outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                        outfile << str;
                        outfile.close();
                    }
                } catch (RMDBError &e) {
                    // 遇到异常，需要打印failure到output.txt文件中，并发异常信息返回给客户端
                    AbortActiveTransaction(&txn_id, context.get());

                    memcpy(data_send, e.what(), e.get_msg_len());
                    data_send[e.get_msg_len()] = '\n';
                    data_send[e.get_msg_len() + 1] = '\0';
                    offset = e.get_msg_len() + 1;

                    // 将报错信息写入output.txt
                    if (output_file_enabled) {
                        std::fstream outfile;
                        outfile.open(sm_manager->db_.name() + "/output.txt", std::ios::out | std::ios::app);
                        outfile << "failure\n";
                        outfile.close();
                    }
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
        if (write(fd, data_send, offset + 1) == -1) {
            break;
        }
        // 如果是单挑语句，需要按照一个完整的事务来执行，所以执行完当前语句后，自动提交事务
        if(context->txn_ != nullptr && context->txn_->get_txn_mode() == false &&
           context->txn_->get_state() != TransactionState::COMMITTED &&
           context->txn_->get_state() != TransactionState::ABORTED)
        {
            txn_manager->commit(context->txn_, context->log_mgr_);
            txn_manager->release_transaction(context->txn_);
            context->txn_ = nullptr;
            txn_id = INVALID_TXN_ID;
        }
    }

    // Clear
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
