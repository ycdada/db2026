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

#include <chrono>

namespace {
constexpr auto LOCK_WAIT_TIMEOUT = std::chrono::seconds(60);
}

LockManager::LockTableShard &LockManager::get_shard(const LockDataId &lock_data_id) {
    return lock_table_shards_[std::hash<LockDataId>{}(lock_data_id) % LOCK_TABLE_SHARD_COUNT];
}

/**
 * @description: 通用加锁实现（waiting 2PL with bounded deadlock prevention）
 * @return true 加锁成功
 */
bool LockManager::lock(Transaction *txn, LockDataId lid, LockMode mode) {
    auto compatible = [](LockMode lhs, LockMode rhs) -> bool {
        if (lhs == LockMode::EXLUCSIVE || rhs == LockMode::EXLUCSIVE) return false;
        if (lhs == LockMode::UPDATE) {
            return rhs == LockMode::SHARED || rhs == LockMode::INTENTION_SHARED;
        }
        if (rhs == LockMode::UPDATE) {
            return lhs == LockMode::SHARED || lhs == LockMode::INTENTION_SHARED;
        }
        if (lhs == LockMode::S_IX) return rhs == LockMode::INTENTION_SHARED;
        if (rhs == LockMode::S_IX) return lhs == LockMode::INTENTION_SHARED;
        if (lhs == LockMode::SHARED) {
            return rhs == LockMode::SHARED || rhs == LockMode::INTENTION_SHARED;
        }
        if (rhs == LockMode::SHARED) {
            return lhs == LockMode::SHARED || lhs == LockMode::INTENTION_SHARED;
        }
        return true;
    };

    auto merge_lock = [](LockMode cur, LockMode req) -> LockMode {
        if (cur == req) return cur;
        if (cur == LockMode::EXLUCSIVE || req == LockMode::EXLUCSIVE) return LockMode::EXLUCSIVE;
        if (cur == LockMode::S_IX || req == LockMode::S_IX) return LockMode::S_IX;
        if (cur == LockMode::UPDATE || req == LockMode::UPDATE) {
            if (cur == LockMode::INTENTION_EXCLUSIVE || req == LockMode::INTENTION_EXCLUSIVE) {
                return LockMode::S_IX;
            }
            return LockMode::UPDATE;
        }
        if ((cur == LockMode::SHARED && req == LockMode::INTENTION_EXCLUSIVE) ||
            (cur == LockMode::INTENTION_EXCLUSIVE && req == LockMode::SHARED)) {
            return LockMode::S_IX;
        }
        if (cur == LockMode::INTENTION_EXCLUSIVE || req == LockMode::INTENTION_EXCLUSIVE) {
            return LockMode::INTENTION_EXCLUSIVE;
        }
        if (cur == LockMode::SHARED || req == LockMode::SHARED) return LockMode::SHARED;
        return LockMode::INTENTION_SHARED;
    };

    auto covers = [&](LockMode held, LockMode req) -> bool {
        return merge_lock(held, req) == held;
    };

    auto group_from_lock = [](LockMode lock_mode) -> GroupLockMode {
        switch (lock_mode) {
            case LockMode::INTENTION_SHARED: return GroupLockMode::IS;
            case LockMode::INTENTION_EXCLUSIVE: return GroupLockMode::IX;
            case LockMode::SHARED: return GroupLockMode::S;
            case LockMode::UPDATE: return GroupLockMode::U;
            case LockMode::S_IX: return GroupLockMode::SIX;
            case LockMode::EXLUCSIVE: return GroupLockMode::X;
        }
        return GroupLockMode::NON_LOCK;
    };

    auto recompute_group = [](LockRequestQueue &queue) {
        bool has_is = false, has_ix = false, has_s = false, has_u = false, has_six = false, has_x = false;
        for (auto &r : queue.request_queue_) {
            if (!r.granted_) continue;
            switch (r.lock_mode_) {
                case LockMode::INTENTION_SHARED:   has_is = true; break;
                case LockMode::INTENTION_EXCLUSIVE: has_ix = true; break;
                case LockMode::SHARED:              has_s = true; break;
                case LockMode::UPDATE:              has_u = true; break;
                case LockMode::S_IX:                has_six = true; break;
                case LockMode::EXLUCSIVE:           has_x = true; break;
            }
        }
        if (has_x) queue.group_lock_mode_ = GroupLockMode::X;
        else if (has_six || (has_s && has_ix)) queue.group_lock_mode_ = GroupLockMode::SIX;
        else if (has_u) queue.group_lock_mode_ = GroupLockMode::U;
        else if (has_s) queue.group_lock_mode_ = GroupLockMode::S;
        else if (has_ix) queue.group_lock_mode_ = GroupLockMode::IX;
        else if (has_is) queue.group_lock_mode_ = GroupLockMode::IS;
        else queue.group_lock_mode_ = GroupLockMode::NON_LOCK;
    };

    // 2PL: SHRINKING 阶段不可申请新锁
    if (txn->get_state() == TransactionState::SHRINKING) {
        throw TransactionAbortException(txn->get_transaction_id(), AbortReason::LOCK_ON_SHIRINKING);
    }

    auto request_compatible = [&](const LockRequestQueue &queue, txn_id_t txn_id, LockMode requested_mode) {
        for (auto &req : queue.request_queue_) {
            if (!req.granted_ || req.txn_id_ == txn_id) continue;
            if (!compatible(requested_mode, req.lock_mode_)) {
                return false;
            }
        }
        return true;
    };

    auto request_can_grant_in_order = [&](const LockRequestQueue &queue,
                                          std::list<LockRequest>::const_iterator self_it,
                                          LockMode requested_mode) {
        for (auto it = queue.request_queue_.cbegin(); it != queue.request_queue_.cend(); ++it) {
            if (it == self_it) {
                return true;
            }
            if (it->txn_id_ == self_it->txn_id_) {
                continue;
            }
            if (!compatible(requested_mode, it->lock_mode_)) {
                return false;
            }
        }
        return true;
    };

    auto &shard = get_shard(lid);
    std::unique_lock<std::mutex> guard(shard.latch_);

    auto &queue = shard.lock_table_[lid];
    if (queue.request_queue_.empty()) {
        queue.request_queue_.emplace_back(txn->get_transaction_id(), mode);
        queue.request_queue_.back().granted_ = true;
        queue.group_lock_mode_ = group_from_lock(mode);
        txn->get_lock_set()->insert(lid);
        return true;
    }

    LockRequest *own_request = nullptr;
    for (auto &req : queue.request_queue_) {
        if (req.txn_id_ == txn->get_transaction_id() && req.granted_) {
            own_request = &req;
            break;
        }
    }

    LockMode requested_mode = own_request == nullptr ? mode : merge_lock(own_request->lock_mode_, mode);
    if (own_request != nullptr && covers(own_request->lock_mode_, mode)) {
        return true;
    }

    if (own_request != nullptr) {
        queue.request_queue_.emplace_back(txn->get_transaction_id(), requested_mode);
        auto request_it = std::prev(queue.request_queue_.end());
        bool granted = queue.cv_.wait_for(guard, LOCK_WAIT_TIMEOUT, [&]() {
            return request_compatible(queue, txn->get_transaction_id(), requested_mode);
        });
        if (!granted) {
            queue.request_queue_.erase(request_it);
            recompute_group(queue);
            queue.cv_.notify_all();
            throw TransactionAbortException(txn->get_transaction_id(), AbortReason::DEADLOCK_PREVENTION);
        }
        own_request->lock_mode_ = requested_mode;
        queue.request_queue_.erase(request_it);
        recompute_group(queue);
        queue.cv_.notify_all();
        return true;
    }

    queue.request_queue_.emplace_back(txn->get_transaction_id(), requested_mode);
    auto request_it = std::prev(queue.request_queue_.end());
    bool granted = queue.cv_.wait_for(guard, LOCK_WAIT_TIMEOUT, [&]() {
        return request_can_grant_in_order(queue, request_it, requested_mode);
    });
    if (!granted) {
        queue.request_queue_.erase(request_it);
        recompute_group(queue);
        queue.cv_.notify_all();
        if (queue.request_queue_.empty()) {
            shard.lock_table_.erase(lid);
        }
        throw TransactionAbortException(txn->get_transaction_id(), AbortReason::DEADLOCK_PREVENTION);
    }
    request_it->granted_ = true;
    recompute_group(queue);
    txn->get_lock_set()->insert(lid);
    return true;
}

bool LockManager::lock_shared_on_record(Transaction *txn, const Rid &rid, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, rid, LockDataType::RECORD), LockMode::SHARED);
}

bool LockManager::lock_update_on_record(Transaction *txn, const Rid &rid, int tab_fd) {
    return lock(txn, LockDataId(tab_fd, rid, LockDataType::RECORD), LockMode::UPDATE);
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
bool LockManager::unlock(Transaction *txn, LockDataId lock_data_id, bool enter_shrinking) {
    auto &shard = get_shard(lock_data_id);
    std::lock_guard<std::mutex> guard(shard.latch_);

    auto it = shard.lock_table_.find(lock_data_id);
    if (it == shard.lock_table_.end()) return false;

    auto &queue = it->second;
    if (queue.request_queue_.size() == 1) {
        auto req_it = queue.request_queue_.begin();
        if (req_it->txn_id_ == txn->get_transaction_id() && req_it->granted_) {
            txn->get_lock_set()->erase(lock_data_id);
            if (enter_shrinking && (txn->get_state() == TransactionState::GROWING ||
                                    txn->get_state() == TransactionState::DEFAULT)) {
                txn->set_state(TransactionState::SHRINKING);
            }
            shard.lock_table_.erase(it);
            return true;
        }
    }

    for (auto req_it = queue.request_queue_.begin(); req_it != queue.request_queue_.end(); ++req_it) {
        if (req_it->txn_id_ == txn->get_transaction_id() && req_it->granted_) {
            queue.request_queue_.erase(req_it);

            // 重新计算组锁模式（遍历所有已授予请求）
            bool has_is = false, has_ix = false, has_s = false, has_u = false, has_six = false, has_x = false;
            for (auto &r : queue.request_queue_) {
                if (!r.granted_) continue;
                switch (r.lock_mode_) {
                    case LockMode::INTENTION_SHARED:   has_is = true; break;
                    case LockMode::INTENTION_EXCLUSIVE: has_ix = true; break;
                    case LockMode::SHARED:              has_s = true;  break;
                    case LockMode::UPDATE:              has_u = true;  break;
                    case LockMode::S_IX:                has_six = true; break;
                    case LockMode::EXLUCSIVE:            has_x = true;  break;
                }
            }
            if (has_x)                queue.group_lock_mode_ = GroupLockMode::X;
            else if (has_six || (has_s && has_ix))
                                      queue.group_lock_mode_ = GroupLockMode::SIX;
            else if (has_u)           queue.group_lock_mode_ = GroupLockMode::U;
            else if (has_s)           queue.group_lock_mode_ = GroupLockMode::S;
            else if (has_ix)          queue.group_lock_mode_ = GroupLockMode::IX;
            else if (has_is)          queue.group_lock_mode_ = GroupLockMode::IS;
            else                      queue.group_lock_mode_ = GroupLockMode::NON_LOCK;

            queue.cv_.notify_all();
            break;
        }
    }

    txn->get_lock_set()->erase(lock_data_id);

    // 2PL: 释放锁后事务进入 SHRINKING 阶段
    if (enter_shrinking && (txn->get_state() == TransactionState::GROWING ||
                            txn->get_state() == TransactionState::DEFAULT)) {
        txn->set_state(TransactionState::SHRINKING);
    }

    // 清理空队列
    if (queue.request_queue_.empty()) {
        shard.lock_table_.erase(it);
    } else {
        queue.cv_.notify_all();
    }

    return true;
}
