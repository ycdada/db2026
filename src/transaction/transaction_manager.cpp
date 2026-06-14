/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "transaction_manager.h"
#include "record/rm_file_handle.h"
#include "system/sm_manager.h"

std::unordered_map<txn_id_t, Transaction *> TransactionManager::txn_map = {};

namespace {

std::vector<char> make_index_key(const RmRecord &record, const IndexMeta &index) {
    std::vector<char> key(index.col_tot_len);
    int offset = 0;
    for (int i = 0; i < index.col_num; ++i) {
        memcpy(key.data() + offset, record.data + index.cols[i].offset, index.cols[i].len);
        offset += index.cols[i].len;
    }
    return key;
}

void insert_index_entries(SmManager *sm_manager, const std::string &tab_name, const TabMeta &tab,
                          const RmRecord &record, const Rid &rid, Transaction *txn) {
    for (auto &index : tab.indexes) {
        auto ih = sm_manager->ihs_.at(sm_manager->get_ix_manager()->get_index_name(tab_name, index.cols)).get();
        auto key = make_index_key(record, index);
        ih->insert_entry(key.data(), rid, txn);
    }
}

void delete_index_entries(SmManager *sm_manager, const std::string &tab_name, const TabMeta &tab,
                          const RmRecord &record, Transaction *txn) {
    for (auto &index : tab.indexes) {
        auto ih = sm_manager->ihs_.at(sm_manager->get_ix_manager()->get_index_name(tab_name, index.cols)).get();
        auto key = make_index_key(record, index);
        ih->delete_entry(key.data(), txn);
    }
}

}

/**
 * @description: 事务的开始方法
 * @return {Transaction*} 开始事务的指针
 * @param {Transaction*} txn 事务指针，空指针代表需要创建新事务，否则开始已有事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
Transaction * TransactionManager::begin(Transaction* txn, LogManager* log_manager) {
    // Todo:
    // 1. 判断传入事务参数是否为空指针
    // 2. 如果为空指针，创建新事务
    // 3. 把开始事务加入到全局事务表中
    // 4. 返回当前事务指针
    // 如果需要支持MVCC请在上述过程中添加代码
    if (txn == nullptr) {
        // 创建新事务
        txn_id_t new_txn_id = next_txn_id_++;
        txn = new Transaction(new_txn_id);
        txn->set_state(TransactionState::DEFAULT);

        std::unique_lock<std::mutex> lock(latch_);
        txn_map[new_txn_id] = txn;
    }
    // 如果是已有事务，直接复用
    txn->set_state(TransactionState::GROWING);
    return txn;
}

/**
 * @description: 事务的提交方法
 * @param {Transaction*} txn 需要提交的事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
void TransactionManager::commit(Transaction* txn, LogManager* log_manager) {
    // Todo:
    // 1. 如果存在未提交的写操作，提交所有的写操作
    // 2. 释放所有锁
    // 3. 释放事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态
    // 如果需要支持MVCC请在上述过程中添加代码
    if (txn == nullptr) return;

    auto write_set = txn->get_write_set();
    while (!write_set->empty()) {
        delete write_set->back();
        write_set->pop_back();
    }

    // 释放所有锁
    auto lock_set = txn->get_lock_set();
    for (auto &lock_id : *lock_set) {
        lock_manager_->unlock(txn, lock_id);
    }
    lock_set->clear();

    // 更新事务状态
    txn->set_state(TransactionState::COMMITTED);
}

/**
 * @description: 事务的终止（回滚）方法
 * @param {Transaction *} txn 需要回滚的事务
 * @param {LogManager} *log_manager 日志管理器指针
 */
void TransactionManager::abort(Transaction * txn, LogManager *log_manager) {
    // Todo:
    // 1. 回滚所有写操作
    // 2. 释放所有锁
    // 3. 清空事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态
    // 如果需要支持MVCC请在上述过程中添加代码
    if (txn == nullptr) return;

    auto write_set = txn->get_write_set();
    while (!write_set->empty()) {
        WriteRecord *write_record = write_set->back();
        write_set->pop_back();

        const std::string &tab_name = write_record->GetTableName();
        TabMeta &tab = sm_manager_->db_.get_table(tab_name);
        RmFileHandle *fh = sm_manager_->fhs_.at(tab_name).get();
        Rid rid = write_record->GetRid();

        switch (write_record->GetWriteType()) {
            case WType::INSERT_TUPLE: {
                auto rec = fh->get_record(rid, nullptr);
                delete_index_entries(sm_manager_, tab_name, tab, *rec, txn);
                fh->delete_record(rid, nullptr);
                break;
            }
            case WType::DELETE_TUPLE: {
                RmRecord &old_rec = write_record->GetRecord();
                fh->insert_record(rid, old_rec.data);
                insert_index_entries(sm_manager_, tab_name, tab, old_rec, rid, txn);
                break;
            }
            case WType::UPDATE_TUPLE: {
                auto curr_rec = fh->get_record(rid, nullptr);
                delete_index_entries(sm_manager_, tab_name, tab, *curr_rec, txn);

                RmRecord &old_rec = write_record->GetRecord();
                fh->update_record(rid, old_rec.data, nullptr);
                insert_index_entries(sm_manager_, tab_name, tab, old_rec, rid, txn);
                break;
            }
        }

        delete write_record;
    }

    // 释放所有锁
    auto lock_set = txn->get_lock_set();
    for (auto &lock_id : *lock_set) {
        lock_manager_->unlock(txn, lock_id);
    }
    lock_set->clear();

    // 更新事务状态
    txn->set_state(TransactionState::ABORTED);
}
