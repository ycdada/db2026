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

#include "transaction/transaction.h"
#include "transaction/concurrency/lock_manager.h"
#include "recovery/log_manager.h"

#include <atomic>

class TransactionManager;

// used for data_send
static int const_offset = -1;

class Context {
public:
    Context (LockManager *lock_mgr, LogManager *log_mgr,
            Transaction *txn, char *data_send = nullptr, int *offset = &const_offset,
            TransactionManager *txn_mgr = nullptr, IsolationLevel *session_isolation_level = nullptr,
            std::atomic<bool> *isolation_output_format = nullptr, bool *output_file_enabled = nullptr)
        : lock_mgr_(lock_mgr), log_mgr_(log_mgr), txn_(txn),
          data_send_(data_send), offset_(offset), txn_mgr_(txn_mgr),
          session_isolation_level_(session_isolation_level),
          isolation_output_format_(isolation_output_format),
          output_file_enabled_(output_file_enabled) {
            ellipsis_ = false;
          }

    LockManager *lock_mgr_;
    LogManager *log_mgr_;
    Transaction *txn_;
    char *data_send_;
    int *offset_;
    TransactionManager *txn_mgr_;
    IsolationLevel *session_isolation_level_;
    std::atomic<bool> *isolation_output_format_;
    bool *output_file_enabled_;
    bool ellipsis_;
};
