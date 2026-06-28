/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <cstring>
#include "log_manager.h"

/**
 * @description: 添加日志记录到日志缓冲区中，并返回日志记录号
 * @param {LogRecord*} log_record 要写入缓冲区的日志记录
 * @return {lsn_t} 返回该日志的日志记录号
 */
lsn_t LogManager::add_log_to_buffer(LogRecord* log_record) {
    std::scoped_lock<std::mutex> lock(latch_);
    log_record->lsn_ = global_lsn_.fetch_add(1);
    if (log_buffer_.is_full(log_record->log_tot_len_)) {
        if (log_buffer_.offset_ > 0) {
            disk_manager_->write_log(log_buffer_.buffer_, log_buffer_.offset_);
            persist_lsn_ = log_record->lsn_ - 1;
            log_buffer_.offset_ = 0;
        }
    }
    log_record->serialize(log_buffer_.buffer_ + log_buffer_.offset_);
    log_buffer_.offset_ += log_record->log_tot_len_;
    return log_record->lsn_;
}

/**
 * @description: 把日志缓冲区的内容刷到磁盘中，由于目前只设置了一个缓冲区，因此需要阻塞其他日志操作
 */
void LogManager::flush_log_to_disk() {
    std::scoped_lock<std::mutex> lock(latch_);
    if (log_buffer_.offset_ == 0) {
        return;
    }
    disk_manager_->write_log(log_buffer_.buffer_, log_buffer_.offset_);
    persist_lsn_ = global_lsn_.load() - 1;
    log_buffer_.offset_ = 0;
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
            record = std::make_unique<UpdateLogRecord>();
            break;
        case LogType::INSERT:
            record = std::make_unique<InsertLogRecord>();
            break;
        case LogType::DELETE:
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
