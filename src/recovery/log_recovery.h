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

#include <map>
#include <set>
#include <unordered_map>
#include "log_manager.h"
#include "storage/disk_manager.h"
#include "system/sm_manager.h"

class RedoLogsInPage {
public:
    RedoLogsInPage() { table_file_ = nullptr; }
    RmFileHandle* table_file_;
    std::vector<lsn_t> redo_logs_;   // 在该page上需要redo的操作的lsn
};

class RecoveryManager {
public:
    RecoveryManager(DiskManager* disk_manager, BufferPoolManager* buffer_pool_manager, SmManager* sm_manager) {
        disk_manager_ = disk_manager;
        buffer_pool_manager_ = buffer_pool_manager;
        sm_manager_ = sm_manager;
    }

    void analyze();
    void redo();
    void undo();
private:
    void load_logs();
    int checkpoint_offset();
    void apply_redo(LogRecord *record);
    void apply_undo(LogRecord *record);
    void redo_insert(const std::string &tab_name, const Rid &rid, const RmRecord &record);
    void redo_delete(const std::string &tab_name, const Rid &rid, const RmRecord &record);
    void redo_update(const std::string &tab_name, const Rid &rid, const RmRecord &old_record, const RmRecord &new_record);
    bool page_has_log(const std::string &tab_name, const Rid &rid, lsn_t lsn);
    void ensure_record_slot(RmFileHandle *fh, const Rid &rid);
    bool record_exists(RmFileHandle *fh, const Rid &rid);
    void insert_indexes(const std::string &tab_name, const RmRecord &record, const Rid &rid);
    void delete_indexes(const std::string &tab_name, const RmRecord &record);

    LogBuffer buffer_;                                              // 读入日志
    DiskManager* disk_manager_;                                     // 用来读写文件
    BufferPoolManager* buffer_pool_manager_;                        // 对页面进行读写
    SmManager* sm_manager_;                                         // 访问数据库元数据
    std::vector<std::unique_ptr<LogRecord>> logs_;
    std::set<txn_id_t> committed_txns_;
    std::set<txn_id_t> active_txns_;
    std::vector<LogRecord*> undo_logs_;
};
