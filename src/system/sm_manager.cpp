/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sm_manager.h"

#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <sstream>

#include "index/ix.h"
#include "record/rm.h"
#include "record_printer.h"

/**
 * @description: 判断是否为一个文件夹
 * @return {bool} 返回是否为一个文件夹
 * @param {string&} db_name 数据库文件名称，与文件夹同名
 */
bool SmManager::is_dir(const std::string& db_name) {
    struct stat st;
    return stat(db_name.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

/**
 * @description: 创建数据库，所有的数据库相关文件都放在数据库同名文件夹下
 * @param {string&} db_name 数据库名称
 */
void SmManager::create_db(const std::string& db_name) {
    if (is_dir(db_name)) {
        throw DatabaseExistsError(db_name);
    }
    //为数据库创建一个子目录
    std::string cmd = "mkdir " + db_name;
    if (system(cmd.c_str()) < 0) {  // 创建一个名为db_name的目录
        throw UnixError();
    }
    if (chdir(db_name.c_str()) < 0) {  // 进入名为db_name的目录
        throw UnixError();
    }
    //创建系统目录
    DbMeta *new_db = new DbMeta();
    new_db->name_ = db_name;
    // 将元数据同时加载到内存中的 db_ 对象
    db_.name() = db_name;

    // 注意，此处ofstream会在当前目录创建(如果没有此文件先创建)和打开一个名为DB_META_NAME的文件
    std::ofstream ofs(DB_META_NAME);

    // 将new_db中的信息，按照定义好的operator<<操作符，写入到ofs打开的DB_META_NAME文件中
    ofs << *new_db;  // 注意：此处重载了操作符<<

    delete new_db;

    // 创建日志文件
    disk_manager_->create_file(LOG_FILE_NAME);

    // 回到根目录
    if (chdir("..") < 0) {
        throw UnixError();
    }
}

/**
 * @description: 删除数据库，同时需要清空相关文件以及数据库同名文件夹
 * @param {string&} db_name 数据库名称，与文件夹同名
 */
void SmManager::drop_db(const std::string& db_name) {
    if (!is_dir(db_name)) {
        throw DatabaseNotFoundError(db_name);
    }
    std::string cmd = "rm -r " + db_name;
    if (system(cmd.c_str()) < 0) {
        throw UnixError();
    }
}

/**
 * @description: 打开数据库，找到数据库对应的文件夹，并加载数据库元数据和相关文件
 * @param {string&} db_name 数据库名称，与文件夹同名
 */
void SmManager::open_db(const std::string& db_name) {
    // 加载数据库元数据到内存
    db_.name() = db_name;
    // 进入数据库目录读取元数据文件
    if (chdir(db_name.c_str()) < 0) {
        throw UnixError();
    }
    std::ifstream ifs(DB_META_NAME);
    if (ifs.is_open()) {
        ifs >> db_;
    }
    ifs.close();
    // 回到上级目录
    if (chdir("..") < 0) {
        throw UnixError();
    }
    for (auto &entry : db_.tabs_) {
        auto &tab = entry.second;
        std::string file_path = db_.name() + "/" + tab.name;
        fhs_[tab.name] = rm_manager_->open_file(file_path);
        for (auto &index : tab.indexes) {
            std::string ix_name = ix_manager_->get_index_name(tab.name, index.cols);
            ihs_[ix_name] = ix_manager_->open_index(file_path, index.cols);
        }
    }
}

/**
 * @description: 把数据库相关的元数据刷入磁盘中
 */
void SmManager::flush_meta() {
    // 默认清空文件
    std::ofstream ofs(db_.name() + "/" + DB_META_NAME);
    ofs << db_;
}

void SmManager::create_static_checkpoint(LogManager* log_manager) {
    std::scoped_lock<std::mutex> lock(checkpoint_latch_);
    int checkpoint_start = disk_manager_->get_file_size(db_.name() + "/" + LOG_FILE_NAME);
    if (log_manager != nullptr) {
        log_manager->flush_log_to_disk();
        checkpoint_start = disk_manager_->get_file_size(db_.name() + "/" + LOG_FILE_NAME);
        CheckpointLogRecord checkpoint;
        log_manager->add_log_record(&checkpoint);
        log_manager->flush_log_to_disk();
    }

    for (auto &entry : fhs_) {
        RmFileHandle *fh = entry.second.get();
        fh->flush_file_hdr();
        buffer_pool_manager_->flush_all_pages(fh->GetFd());
    }
    for (auto &entry : ihs_) {
        IxIndexHandle *ih = entry.second.get();
        ih->flush_file_hdr();
        buffer_pool_manager_->flush_all_pages(ih->GetFd());
    }
    flush_meta();

    std::ofstream ofs(db_.name() + "/" + RESTART_FILE_NAME, std::ios::out | std::ios::trunc);
    ofs << std::max(0, checkpoint_start);
}

static std::vector<std::string> split_csv_line(const std::string &line) {
    std::vector<std::string> fields;
    std::string cur;
    bool in_quote = false;
    for (char ch : line) {
        if (ch == '"') {
            in_quote = !in_quote;
        } else if (ch == ',' && !in_quote) {
            fields.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(ch);
        }
    }
    fields.push_back(cur);
    return fields;
}

static std::string trim_csv_field(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
        s.erase(s.begin());
    }
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
    if (s.size() >= 2 && ((s.front() == '\'' && s.back() == '\'') || (s.front() == '"' && s.back() == '"'))) {
        s = s.substr(1, s.size() - 2);
    }
    return s;
}

static bool is_absolute_path(const std::string &path) {
    return !path.empty() && path.front() == '/';
}

static std::string unquote_load_path(std::string path) {
    path = trim_csv_field(std::move(path));
    return path;
}

static bool is_csv_header(const std::vector<std::string> &fields, const TabMeta &tab) {
    if (fields.size() != tab.cols.size()) {
        return false;
    }
    for (size_t i = 0; i < fields.size(); ++i) {
        if (trim_csv_field(fields[i]) != tab.cols[i].name) {
            return false;
        }
    }
    return true;
}

void SmManager::load_table(const std::string& file_name, const std::string& tab_name, Context* context) {
    if (!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    std::string load_path = unquote_load_path(file_name);
    std::ifstream ifs;
    if (is_absolute_path(load_path)) {
        ifs.open(load_path);
    } else {
        ifs.open(db_.name() + "/" + load_path);
        if (!ifs.is_open()) {
            ifs.clear();
            ifs.open(load_path);
        }
    }
    if (!ifs.is_open()) {
        throw FileNotFoundError(load_path);
    }

    TabMeta &tab = db_.get_table(tab_name);
    RmFileHandle *fh = fhs_.at(tab_name).get();
    std::string line;
    bool first_data_line = true;
    while (std::getline(ifs, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        auto fields = split_csv_line(line);
        if (fields.size() != tab.cols.size()) {
            throw InvalidValueCountError();
        }
        if (first_data_line && is_csv_header(fields, tab)) {
            first_data_line = false;
            continue;
        }
        first_data_line = false;

        RmRecord rec(fh->get_file_hdr().record_size);
        for (size_t i = 0; i < fields.size(); ++i) {
            const auto &col = tab.cols[i];
            std::string field = trim_csv_field(fields[i]);
            char *dst = rec.data + col.offset;
            if (col.type == TYPE_INT) {
                int value = std::stoi(field);
                memcpy(dst, &value, sizeof(int));
            } else if (col.type == TYPE_FLOAT) {
                float value = std::stof(field);
                memcpy(dst, &value, sizeof(float));
            } else {
                if ((int)field.size() > col.len) {
                    throw StringOverflowError();
                }
                memset(dst, 0, col.len);
                memcpy(dst, field.data(), field.size());
            }
        }

        for (auto &index : tab.indexes) {
            auto ih = ihs_.at(ix_manager_->get_index_name(tab_name, index.cols)).get();
            std::vector<char> key(index.col_tot_len);
            int offset = 0;
            for (int i = 0; i < index.col_num; ++i) {
                memcpy(key.data() + offset, rec.data + index.cols[i].offset, index.cols[i].len);
                offset += index.cols[i].len;
            }
            std::vector<Rid> result;
            if (ih->get_value(key.data(), &result, context ? context->txn_ : nullptr)) {
                throw RMDBError("Duplicate key in unique index");
            }
        }

        Rid rid = fh->insert_record(rec.data, context);
        if (context != nullptr && context->txn_ != nullptr) {
            context->txn_->append_write_record(new WriteRecord(WType::INSERT_TUPLE, tab_name, rid));
        }
        if (context != nullptr && context->txn_ != nullptr && context->log_mgr_ != nullptr) {
            InsertLogRecord log(context->txn_->get_transaction_id(), rec, rid, tab_name);
            log.prev_lsn_ = context->txn_->get_prev_lsn();
            lsn_t lsn = context->log_mgr_->add_log_record(&log);
            context->txn_->set_prev_lsn(lsn);
        }

        try {
            for (auto &index : tab.indexes) {
                auto ih = ihs_.at(ix_manager_->get_index_name(tab_name, index.cols)).get();
                std::vector<char> key(index.col_tot_len);
                int offset = 0;
                for (int i = 0; i < index.col_num; ++i) {
                    memcpy(key.data() + offset, rec.data + index.cols[i].offset, index.cols[i].len);
                    offset += index.cols[i].len;
                }
                ih->insert_entry(key.data(), rid, context ? context->txn_ : nullptr);
            }
        } catch (...) {
            fh->delete_record(rid, context);
            throw;
        }
    }
}

/**
 * @description: 关闭数据库并把数据落盘
 */
void SmManager::close_db() {
    for (auto &entry : ihs_) {
        ix_manager_->close_index(entry.second.get());
    }
    ihs_.clear();
    for (auto &entry : fhs_) {
        rm_manager_->close_file(entry.second.get());
    }
    fhs_.clear();
    flush_meta();
}

/**
 * @description: 显示所有的表,通过测试需要将其结果写入到output.txt,详情看题目文档
 * @param {Context*} context 
 */
void SmManager::show_tables(Context* context) {
    std::fstream outfile;
    bool write_output = context == nullptr || context->output_file_enabled_ == nullptr || *context->output_file_enabled_;
    if (write_output) {
        outfile.open(db_.name() + "/output.txt", std::ios::out | std::ios::app);
        outfile << "| Tables |\n";
    }
    RecordPrinter printer(1);
    printer.print_separator(context);
    printer.print_record({"Tables"}, context);
    printer.print_separator(context);
    for (auto &entry : db_.tabs_) {
        auto &tab = entry.second;
        printer.print_record({tab.name}, context);
        if (write_output) {
            outfile << "| " << tab.name << " |\n";
        }
    }
    printer.print_separator(context);
    if (write_output) {
        outfile.close();
    }
}

/**
 * @description: 显示表的元数据
 * @param {string&} tab_name 表名称
 * @param {Context*} context 
 */
void SmManager::desc_table(const std::string& tab_name, Context* context) {
    TabMeta &tab = db_.get_table(tab_name);

    std::vector<std::string> captions = {"Field", "Type", "Index"};
    RecordPrinter printer(captions.size());
    // Print header
    printer.print_separator(context);
    printer.print_record(captions, context);
    printer.print_separator(context);
    // Print fields
    for (auto &col : tab.cols) {
        std::vector<std::string> field_info = {col.name, coltype2str(col.type), col.index ? "YES" : "NO"};
        printer.print_record(field_info, context);
    }
    // Print footer
    printer.print_separator(context);
}

void SmManager::show_index(const std::string& tab_name, Context* context) {
    TabMeta &tab = db_.get_table(tab_name);
    std::fstream outfile;
    bool write_output = context == nullptr || context->output_file_enabled_ == nullptr || *context->output_file_enabled_;
    if (write_output) {
        outfile.open(db_.name() + "/output.txt", std::ios::out | std::ios::app);
    }

    RecordPrinter printer(3);
    printer.print_separator(context);
    printer.print_record({"table_name", "unique", "column_name"}, context);
    printer.print_separator(context);
    for (auto &index : tab.indexes) {
        std::string cols = "(";
        for (int i = 0; i < index.col_num; ++i) {
            if (i > 0) cols += ",";
            cols += index.cols[i].name;
        }
        cols += ")";
        printer.print_record({tab_name, "unique", cols}, context);
        if (write_output) {
            outfile << "| " << tab_name << " | unique | " << cols << " |\n";
        }
    }
    printer.print_separator(context);
    if (write_output) {
        outfile.close();
    }
}

/**
 * @description: 创建表
 * @param {string&} tab_name 表的名称
 * @param {vector<ColDef>&} col_defs 表的字段
 * @param {Context*} context 
 */
void SmManager::create_table(const std::string& tab_name, const std::vector<ColDef>& col_defs, Context* context) {
    if (db_.is_table(tab_name)) {
        throw TableExistsError(tab_name);
    }
    // Create table meta
    int curr_offset = 0;
    TabMeta tab;
    tab.name = tab_name;
    for (auto &col_def : col_defs) {
        ColMeta col = {.tab_name = tab_name,
                       .name = col_def.name,
                       .type = col_def.type,
                       .len = col_def.len,
                       .offset = curr_offset,
                       .index = false};
        curr_offset += col_def.len;
        tab.cols.push_back(col);
    }
    // Create & open record file
    int record_size = curr_offset;  // record_size就是col meta所占的大小（表的元数据也是以记录的形式进行存储的）
    std::string file_path = db_.name() + "/" + tab_name;
    rm_manager_->create_file(file_path, record_size);
    db_.tabs_[tab_name] = tab;
    // fhs_[tab_name] = rm_manager_->open_file(tab_name);
    fhs_.emplace(tab_name, rm_manager_->open_file(file_path));

    flush_meta();
}

/**
 * @description: 删除表
 * @param {string&} tab_name 表的名称
 * @param {Context*} context
 */
void SmManager::drop_table(const std::string& tab_name, Context* context) {
    if (!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    std::string file_path = db_.name() + "/" + tab_name;
    // 关闭并删除表的数据文件
    auto it_fh = fhs_.find(tab_name);
    if (it_fh != fhs_.end()) {
        buffer_pool_manager_->flush_all_pages(it_fh->second->GetFd());
        buffer_pool_manager_->delete_all_pages(it_fh->second->GetFd());
        disk_manager_->close_file(it_fh->second->GetFd());
        fhs_.erase(it_fh);
    }
    disk_manager_->destroy_file(file_path);
    // 清理索引
    TabMeta &tab = db_.get_table(tab_name);
    for (auto &index : tab.indexes) {
        std::string ix_name = ix_manager_->get_index_name(tab_name, index.cols);
        std::string ix_path = db_.name() + "/" + ix_name;
        auto it_ih = ihs_.find(ix_name);
        if (it_ih != ihs_.end()) {
            ix_manager_->close_index(it_ih->second.get());
            ihs_.erase(it_ih);
        }
        disk_manager_->destroy_file(ix_path);
    }
    // 从元数据中删除表
    db_.tabs_.erase(tab_name);
    flush_meta();
}

/**
 * @description: 创建索引
 * @param {string&} tab_name 表的名称
 * @param {vector<string>&} col_names 索引包含的字段名称
 * @param {Context*} context
 */
void SmManager::create_index(const std::string& tab_name, const std::vector<std::string>& col_names, Context* context) {
    if (!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    TabMeta &tab = db_.get_table(tab_name);
    if (tab.is_index(col_names)) {
        throw IndexExistsError(tab_name, col_names);
    }

    std::vector<ColMeta> index_cols;
    int col_tot_len = 0;
    for (auto &col_name : col_names) {
        auto col = tab.get_col(col_name);
        index_cols.push_back(*col);
        col_tot_len += col->len;
    }

    std::string file_path = db_.name() + "/" + tab_name;
    ix_manager_->create_index(file_path, index_cols);
    std::string ix_name = ix_manager_->get_index_name(tab_name, index_cols);
    auto ih = ix_manager_->open_index(file_path, index_cols);

    try {
        RmFileHandle *fh = fhs_.at(tab_name).get();
        for (RmScan scan(fh); !scan.is_end(); scan.next()) {
            auto rec = fh->get_record(scan.rid(), context);
            std::vector<char> key(col_tot_len);
            int offset = 0;
            for (auto &col : index_cols) {
                memcpy(key.data() + offset, rec->data + col.offset, col.len);
                offset += col.len;
            }
            std::vector<Rid> result;
            if (ih->get_value(key.data(), &result, context ? context->txn_ : nullptr)) {
                throw RMDBError("Duplicate key in unique index");
            }
            ih->insert_entry(key.data(), scan.rid(), context ? context->txn_ : nullptr);
        }
    } catch (...) {
        ix_manager_->close_index(ih.get());
        ix_manager_->destroy_index(file_path, index_cols);
        throw;
    }

    IndexMeta index_meta;
    index_meta.tab_name = tab_name;
    index_meta.col_tot_len = col_tot_len;
    index_meta.col_num = static_cast<int>(index_cols.size());
    index_meta.cols = index_cols;
    tab.indexes.push_back(index_meta);
    for (auto &col_name : col_names) {
        tab.get_col(col_name)->index = true;
    }
    ihs_[ix_name] = std::move(ih);
    flush_meta();
}

/**
 * @description: 删除索引
 * @param {string&} tab_name 表名称
 * @param {vector<string>&} col_names 索引包含的字段名称
 * @param {Context*} context
 */
void SmManager::drop_index(const std::string& tab_name, const std::vector<std::string>& col_names, Context* context) {
    if (!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    TabMeta &tab = db_.get_table(tab_name);
    auto index = tab.get_index_meta(col_names);
    std::vector<ColMeta> index_cols = index->cols;
    std::string ix_name = ix_manager_->get_index_name(tab_name, index_cols);
    auto ih = ihs_.find(ix_name);
    if (ih != ihs_.end()) {
        ix_manager_->close_index(ih->second.get());
        ihs_.erase(ih);
    }
    std::string ix_path = db_.name() + "/" + ix_name;
    disk_manager_->destroy_file(ix_path);
    tab.indexes.erase(index);
    for (auto &col : tab.cols) {
        bool still_indexed = false;
        for (auto &idx : tab.indexes) {
            for (auto &idx_col : idx.cols) {
                if (idx_col.name == col.name) {
                    still_indexed = true;
                    break;
                }
            }
            if (still_indexed) break;
        }
        col.index = still_indexed;
    }
    flush_meta();
}

/**
 * @description: 删除索引
 * @param {string&} tab_name 表名称
 * @param {vector<ColMeta>&} 索引包含的字段元数据
 * @param {Context*} context
 */
void SmManager::drop_index(const std::string& tab_name, const std::vector<ColMeta>& cols, Context* context) {
    std::vector<std::string> col_names;
    for (auto &col : cols) {
        col_names.push_back(col.name);
    }
    drop_index(tab_name, col_names, context);
}
