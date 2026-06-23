/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "lock_manager.h"

/**
 * @description: 通用加锁实现（no-wait 2PL）
 * @return true 加锁成功
 */
bool LockManager::lock(Transaction *txn, LockDataId lid, LockMode mode) {
    // 兼容性检测 lambda
    auto compatible = [](LockMode req, GroupLockMode grp) -> bool {
        switch (grp) {
            case GroupLockMode::NON_LOCK: return true;
            case GroupLockMode::IS:       return req != LockMode::EXLUCSIVE;
            case GroupLockMode::IX:       return req == LockMode::INTENTION_SHARED ||
                                                 req == LockMode::INTENTION_EXCLUSIVE;
            case GroupLockMode::S:        return req == LockMode::INTENTION_SHARED ||
                                                 req == LockMode::SHARED;
            case GroupLockMode::SIX:      return req == LockMode::INTENTION_SHARED;
            case GroupLockMode::X:        return false;
        }
        return false;
    };

    // 组锁模式升级 lambda
    auto promote = [](GroupLockMode cur, LockMode req) -> GroupLockMode {
        if (cur == GroupLockMode::X) return GroupLockMode::X;
        if (req == LockMode::EXLUCSIVE) return GroupLockMode::X;

        if (cur == GroupLockMode::SIX || req == LockMode::S_IX) return GroupLockMode::SIX;
        if ((cur == GroupLockMode::S && req == LockMode::INTENTION_EXCLUSIVE) ||
            (cur == GroupLockMode::IX && req == LockMode::SHARED))
            return GroupLockMode::SIX;

        if (req == LockMode::SHARED || cur == GroupLockMode::S) return GroupLockMode::S;
        if (req == LockMode::INTENTION_EXCLUSIVE || cur == GroupLockMode::IX) return GroupLockMode::IX;
        if (req == LockMode::INTENTION_SHARED || cur == GroupLockMode::IS) return GroupLockMode::IS;

        return GroupLockMode::NON_LOCK;
    };

    // 2PL: SHRINKING 阶段不可申请新锁
    if (txn->get_state() == TransactionState::SHRINKING) {
        throw TransactionAbortException(txn->get_transaction_id(), AbortReason::LOCK_ON_SHIRINKING);
    }

    std::lock_guard<std::mutex> guard(latch_);

    auto &queue = lock_table_[lid];

    // 如果事务已持有相同锁，直接返回
    for (auto &req : queue.request_queue_) {
        if (req.txn_id_ == txn->get_transaction_id() && req.granted_) {
            if (req.lock_mode_ == mode) return true;
        }
    }

    // 检查兼容性（no-wait：不兼容则事务回滚）
    if (!compatible(mode, queue.group_lock_mode_)) {
        throw TransactionAbortException(txn->get_transaction_id(), AbortReason::DEADLOCK_PREVENTION);
    }

    // 授予锁
    queue.request_queue_.emplace_back(txn->get_transaction_id(), mode);
    queue.request_queue_.back().granted_ = true;
    queue.group_lock_mode_ = promote(queue.group_lock_mode_, mode);
    txn->get_lock_set()->insert(lid);
    return true;
}

bool LockManager::lock_shared_on_record(Transaction *txn, const Rid &rid, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, rid, LockDataType::RECORD), LockMode::SHARED);
}

bool LockManager::lock_exclusive_on_record(Transaction *txn, const Rid &rid, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, rid, LockDataType::RECORD), LockMode::EXLUCSIVE);
}

bool LockManager::lock_shared_on_table(Transaction *txn, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, LockDataType::TABLE), LockMode::SHARED);
}

bool LockManager::lock_exclusive_on_table(Transaction *txn, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, LockDataType::TABLE), LockMode::EXLUCSIVE);
}

bool LockManager::lock_IS_on_table(Transaction *txn, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, LockDataType::TABLE), LockMode::INTENTION_SHARED);
}

bool LockManager::lock_IX_on_table(Transaction *txn, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, LockDataType::TABLE), LockMode::INTENTION_EXCLUSIVE);
}

/**
 * @description: 释放锁
 * @return true 解锁成功
 */
bool LockManager::unlock(Transaction *txn, LockDataId lock_data_id) {
    std::lock_guard<std::mutex> guard(latch_);

    auto it = lock_table_.find(lock_data_id);
    if (it == lock_table_.end()) return false;

    auto &queue = it->second;
    for (auto req_it = queue.request_queue_.begin(); req_it != queue.request_queue_.end(); ++req_it) {
        if (req_it->txn_id_ == txn->get_transaction_id() && req_it->granted_) {
            queue.request_queue_.erase(req_it);

            // 重新计算组锁模式（遍历所有已授予请求）
            bool has_is = false, has_ix = false, has_s = false, has_six = false, has_x = false;
            for (auto &r : queue.request_queue_) {
                if (!r.granted_) continue;
                switch (r.lock_mode_) {
                    case LockMode::INTENTION_SHARED:   has_is = true; break;
                    case LockMode::INTENTION_EXCLUSIVE: has_ix = true; break;
                    case LockMode::SHARED:              has_s = true;  break;
                    case LockMode::S_IX:                has_six = true; break;
                    case LockMode::EXLUCSIVE:            has_x = true;  break;
                }
            }
            if (has_x)                queue.group_lock_mode_ = GroupLockMode::X;
            else if (has_six || (has_s && has_ix))
                                      queue.group_lock_mode_ = GroupLockMode::SIX;
            else if (has_s)           queue.group_lock_mode_ = GroupLockMode::S;
            else if (has_ix)          queue.group_lock_mode_ = GroupLockMode::IX;
            else if (has_is)          queue.group_lock_mode_ = GroupLockMode::IS;
            else                      queue.group_lock_mode_ = GroupLockMode::NON_LOCK;

            break;
        }
    }

    txn->get_lock_set()->erase(lock_data_id);

    // 2PL: 释放锁后事务进入 SHRINKING 阶段
    if (txn->get_state() == TransactionState::GROWING ||
        txn->get_state() == TransactionState::DEFAULT) {
        txn->set_state(TransactionState::SHRINKING);
    }

    // 清理空队列
    if (queue.request_queue_.empty()) {
        lock_table_.erase(it);
    }

    return true;
}
