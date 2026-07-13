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
#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "disk_manager.h"
#include "errors.h"
#include "page.h"
#include "replacer/lru_replacer.h"
#include "replacer/replacer.h"

class LogManager;

class BufferPoolManager {
   private:
    struct Shard {
        std::unordered_map<PageId, frame_id_t, PageIdHash> page_table_;
        std::list<frame_id_t> free_list_;
        std::unique_ptr<Replacer> replacer_;
        std::mutex latch_;

        explicit Shard(size_t pool_size) : replacer_(std::make_unique<LRUReplacer>(pool_size)) {}
    };

    size_t pool_size_;      // buffer_pool中可容纳页面的个数，即帧的个数
    size_t shard_count_;
    Page *pages_;           // buffer_pool中的Page对象数组，在构造空间中申请内存空间，在析构函数中释放，大小为BUFFER_POOL_SIZE
    std::vector<std::unique_ptr<Shard>> shards_;
    DiskManager *disk_manager_;
    LogManager *log_manager_ = nullptr;

   public:
    BufferPoolManager(size_t pool_size, DiskManager *disk_manager)
        : pool_size_(pool_size), shard_count_(choose_shard_count(pool_size)), disk_manager_(disk_manager) {
        // 为buffer pool分配一块连续的内存空间
        pages_ = new Page[pool_size_];
        shards_.reserve(shard_count_);
        for (size_t i = 0; i < shard_count_; ++i) {
            shards_.emplace_back(std::make_unique<Shard>(pool_size_));
        }
        // 初始化时，所有的page都在对应shard的free_list_中
        for (size_t i = 0; i < pool_size_; ++i) {
            shards_[i % shard_count_]->free_list_.emplace_back(static_cast<frame_id_t>(i));
        }
    }

    ~BufferPoolManager() {
        delete[] pages_;
    }

    /**
     * @description: 将目标页面标记为脏页
     * @param {Page*} page 脏页
     */
    static void mark_dirty(Page* page) { page->is_dirty_ = true; }

    void set_log_manager(LogManager *log_manager) { log_manager_ = log_manager; }

   public: 
    Page* fetch_page(PageId page_id);

    bool unpin_page(PageId page_id, bool is_dirty);

    bool flush_page(PageId page_id);

    Page* new_page(PageId* page_id);

    Page* new_page_at(PageId page_id);

    bool delete_page(PageId page_id);

    void flush_all_pages(int fd);

    void delete_all_pages(int fd);

   private:
    static size_t choose_shard_count(size_t pool_size);

    size_t shard_index(PageId page_id) const;

    Shard& get_shard(PageId page_id) const;

    bool find_victim_page(Shard& shard, frame_id_t* frame_id);

    void update_page(Shard& shard, Page* page, PageId new_page_id, frame_id_t new_frame_id);

    void flush_page_log(Page *page);
};
