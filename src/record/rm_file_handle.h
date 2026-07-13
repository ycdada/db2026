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

#include <assert.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "bitmap.h"
#include "common/context.h"
#include "rm_defs.h"

class RmManager;

/* 对表数据文件中的页面进行封装 */
struct RmPageHandle {
    const RmFileHdr *file_hdr;  // 当前页面所在文件的文件头指针
    Page *page;                 // 页面的实际数据，包括页面存储的数据、元信息等
    RmPageHdr *page_hdr;        // page->data的第一部分，存储页面元信息，指针指向首地址，长度为sizeof(RmPageHdr)
    char *bitmap;               // page->data的第二部分，存储页面的bitmap，指针指向首地址，长度为file_hdr->bitmap_size
    char *slots;                // page->data的第三部分，存储表的记录，指针指向首地址，每个slot的长度为file_hdr->record_size

    RmPageHandle(const RmFileHdr *fhdr_, Page *page_) : file_hdr(fhdr_), page(page_) {
        page_hdr = reinterpret_cast<RmPageHdr *>(page->get_data() + page->OFFSET_PAGE_HDR);
        bitmap = page->get_data() + sizeof(RmPageHdr) + page->OFFSET_PAGE_HDR;
        slots = bitmap + file_hdr->bitmap_size;
    }

    // 返回指定slot_no的slot存储收地址
    char* get_slot(int slot_no) const {
        return slots + slot_no * file_hdr->record_size;  // slots的首地址 + slot个数 * 每个slot的大小(每个record的大小)
    }
};

/* 每个RmFileHandle对应一个表的数据文件，里面有多个page，每个page的数据封装在RmPageHandle中 */
class RmFileHandle {
    friend class RmScan;    
    friend class RmManager;

   private:
    DiskManager *disk_manager_;
    BufferPoolManager *buffer_pool_manager_;
    int fd_;        // 打开文件后产生的文件句柄
    RmFileHdr file_hdr_;    // 文件头，维护当前表文件的元数据
    mutable std::shared_mutex latch_;
    std::string file_name_;
    mutable bool closed_ = false;

    struct SharedRecordState {
        mutable std::shared_mutex latch;
        RmFileHdr file_hdr{};
        bool header_loaded = false;
        bool records_loaded = false;
        size_t active_handles = 0;
        std::unordered_map<int64_t, std::unique_ptr<RmRecord>> records;
    };

    std::shared_ptr<SharedRecordState> shared_state_;
    static std::mutex shared_states_latch_;
    static std::unordered_map<std::string, std::weak_ptr<SharedRecordState>> shared_states_;

   public:
    RmFileHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd)
        : disk_manager_(disk_manager), buffer_pool_manager_(buffer_pool_manager), fd_(fd),
          file_name_(disk_manager_->get_file_name(fd)), shared_state_(get_shared_state(file_name_)) {
        // 注意：这里从磁盘中读出文件描述符为fd的文件的file_hdr，读到内存中
        // 这里实际就是初始化file_hdr，只不过是从磁盘中读出进行初始化
        // init file_hdr_
        RmFileHdr disk_file_hdr{};
        disk_manager_->read_page(fd, RM_FILE_HDR_PAGE, (char *)&disk_file_hdr, sizeof(disk_file_hdr));
        {
            std::unique_lock<std::shared_mutex> guard(shared_state_->latch);
            if (!shared_state_->header_loaded) {
                shared_state_->file_hdr = disk_file_hdr;
                shared_state_->header_loaded = true;
            }
            file_hdr_ = shared_state_->file_hdr;
            shared_state_->active_handles++;
        }
        // disk_manager管理的fd对应的文件中，设置从file_hdr_.num_pages开始分配page_no
        disk_manager_->set_fd2pageno(fd, file_hdr_.num_pages);
        load_memory_records();
    }

    RmFileHdr get_file_hdr() const {
        std::shared_lock<std::shared_mutex> handle_guard(latch_);
        ensure_open_locked();
        std::shared_lock<std::shared_mutex> guard(shared_state_->latch);
        return shared_state_->file_hdr;
    }
    int GetFd() {
        std::shared_lock<std::shared_mutex> handle_guard(latch_);
        ensure_open_locked();
        return fd_;
    }
    void flush_file_hdr() {
        std::shared_lock<std::shared_mutex> handle_guard(latch_);
        ensure_open_locked();
        std::shared_lock<std::shared_mutex> guard(shared_state_->latch);
        disk_manager_->write_page(fd_, RM_FILE_HDR_PAGE, (char *)&shared_state_->file_hdr,
                                  sizeof(shared_state_->file_hdr));
    }

    /* 判断指定位置上是否已经存在一条记录，通过Bitmap来判断 */
    bool is_record(const Rid &rid) const;

    std::unique_ptr<RmRecord> get_record(const Rid &rid, Context *context) const;

    Rid insert_record(char *buf, Context *context);

    void insert_record(const Rid &rid, char *buf);

    void delete_record(const Rid &rid, Context *context);

    void update_record(const Rid &rid, char *buf, Context *context);

    void set_page_lsn(const Rid &rid, lsn_t lsn);

    void reset_data_pages();

    void reserve_memory_records(size_t count);

    std::vector<Rid> snapshot_rids() const;

    RmPageHandle create_new_page_handle();

    RmPageHandle fetch_page_handle(int page_no) const;

   private:
    static std::shared_ptr<SharedRecordState> get_shared_state(const std::string &file_name);

    void detach_shared_state() const;

    void ensure_open_locked() const {
        if (closed_) {
            throw RMDBError("Record file handle is closed: " + file_name_);
        }
    }

    RmPageHandle create_new_page_handle_unlocked();

    RmPageHandle fetch_page_handle_unlocked(int page_no) const;

    RmPageHandle create_page_handle();

    void release_page_handle(RmPageHandle &page_handle);

    void load_memory_records();

    void remove_page_from_free_list(RmPageHandle &page_handle);

    bool record_exists_on_page(const Rid &rid, const RmPageHandle &page_handle) const;

    static int64_t memory_key(const Rid &rid) {
        return (static_cast<int64_t>(rid.page_no) << 32) | static_cast<uint32_t>(rid.slot_no);
    }

    static Rid key_to_rid(int64_t key) {
        return Rid{static_cast<int>(key >> 32), static_cast<int>(key & 0xffffffff)};
    }
};
