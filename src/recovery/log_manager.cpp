/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <algorithm>
#include <cstring>
#include "errors.h"
#include "log_manager.h"

namespace {

bool advance_checked(size_t &offset, size_t amount, size_t total_len) {
    if (amount > total_len || offset > total_len - amount) {
        return false;
    }
    offset += amount;
    return true;
}

bool validate_record_with_table_name(const char *src, size_t total_len, bool has_two_records) {
    size_t offset = OFFSET_LOG_DATA;
    for (int i = 0; i < (has_two_records ? 2 : 1); ++i) {
        if (!advance_checked(offset, sizeof(int), total_len)) {
            return false;
        }
        int rec_size = *reinterpret_cast<const int *>(src + offset - sizeof(int));
        if (rec_size < 0 || !advance_checked(offset, static_cast<size_t>(rec_size), total_len)) {
            return false;
        }
    }
    if (!advance_checked(offset, sizeof(Rid), total_len) ||
        !advance_checked(offset, sizeof(size_t), total_len)) {
        return false;
    }
    size_t table_name_size = *reinterpret_cast<const size_t *>(src + offset - sizeof(size_t));
    if (!advance_checked(offset, table_name_size, total_len)) {
        return false;
    }
    return offset == total_len;
}

}  // namespace

/**
 * @description: 添加日志记录到日志缓冲区中，并返回日志记录号
 * @param {LogRecord*} log_record 要写入缓冲区的日志记录
 * @return {lsn_t} 返回该日志的日志记录号
 */
lsn_t LogManager::add_log_to_buffer(LogRecord* log_record) {
    std::unique_lock<std::mutex> lock(latch_);
    if (log_record->log_tot_len_ > LOG_BUFFER_SIZE) {
        throw InternalError("Log record is larger than log buffer");
    }
    while (log_buffer_.is_full(log_record->log_tot_len_)) {
        flush_buffer_locked(lock);
    }
    log_record->lsn_ = global_lsn_.fetch_add(1);
    log_record->serialize(log_buffer_.buffer_ + log_buffer_.offset_);
    log_buffer_.offset_ += log_record->log_tot_len_;
    log_buffer_max_lsn_ = log_record->lsn_;
    return log_record->lsn_;
}

/**
 * @description: 把当前日志缓冲区内容刷到磁盘。调用方必须持有latch_。
 */
void LogManager::flush_buffer_locked(std::unique_lock<std::mutex> &lock) {
    while (flush_in_progress_) {
        flush_cv_.wait(lock);
    }
    if (log_buffer_.offset_ == 0) {
        return;
    }

    std::memcpy(flush_buffer_->buffer_, log_buffer_.buffer_, log_buffer_.offset_);
    flush_buffer_->offset_ = log_buffer_.offset_;
    lsn_t flush_lsn = log_buffer_max_lsn_;
    log_buffer_.offset_ = 0;
    log_buffer_max_lsn_ = INVALID_LSN;
    flush_in_progress_ = true;

    lock.unlock();
    try {
        disk_manager_->write_log(flush_buffer_->buffer_, flush_buffer_->offset_);
    } catch (...) {
        lock.lock();
        flush_in_progress_ = false;
        lock.unlock();
        flush_cv_.notify_all();
        throw;
    }
    lock.lock();

    persist_lsn_ = std::max(persist_lsn_, flush_lsn);
    flush_in_progress_ = false;
    lock.unlock();
    flush_cv_.notify_all();
    lock.lock();
}

/**
 * @description: 把日志缓冲区的内容刷到磁盘中。若指定target_lsn，则等待该LSN已持久化。
 */
void LogManager::flush_log_to_disk(lsn_t target_lsn) {
    std::unique_lock<std::mutex> lock(latch_);
    if (target_lsn == INVALID_LSN) {
        flush_buffer_locked(lock);
        return;
    }

    while (target_lsn > persist_lsn_) {
        if (flush_in_progress_) {
            flush_cv_.wait(lock);
            continue;
        }
        if (log_buffer_.offset_ == 0) {
            return;
        }
        flush_buffer_locked(lock);
    }
}

std::unique_ptr<LogRecord> LogManager::deserialize_log_record(const char *src, int len) {
    if (len < LOG_HEADER_SIZE) {
        return nullptr;
    }
    auto type = *reinterpret_cast<const LogType *>(src + OFFSET_LOG_TYPE);
    auto total_len = *reinterpret_cast<const uint32_t *>(src + OFFSET_LOG_TOT_LEN);
    if (total_len < LOG_HEADER_SIZE || total_len > static_cast<uint32_t>(len)) {
        return nullptr;
    }

    std::unique_ptr<LogRecord> record;
    switch (type) {
        case LogType::UPDATE:
            if (!validate_record_with_table_name(src, total_len, true)) {
                return nullptr;
            }
            record = std::make_unique<UpdateLogRecord>();
            break;
        case LogType::INSERT:
            if (!validate_record_with_table_name(src, total_len, false)) {
                return nullptr;
            }
            record = std::make_unique<InsertLogRecord>();
            break;
        case LogType::DELETE:
            if (!validate_record_with_table_name(src, total_len, false)) {
                return nullptr;
            }
            record = std::make_unique<DeleteLogRecord>();
            break;
        case LogType::begin:
            record = std::make_unique<BeginLogRecord>();
            break;
        case LogType::commit:
            record = std::make_unique<CommitLogRecord>();
            break;
        case LogType::ABORT:
            record = std::make_unique<AbortLogRecord>();
            break;
        case LogType::CHECKPOINT:
            record = std::make_unique<CheckpointLogRecord>();
            break;
        default:
            return nullptr;
    }
    record->deserialize(src);
    return record;
}
