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
#include <memory>
#include <new>
#include <unordered_map>
#include <vector>

#include "disk_manager.h"
#include "errors.h"
#include "page.h"
#include "replacer/lru_replacer.h"
#include "replacer/replacer.h"

class BufferPoolManager {
   private:
    struct Shard {
        std::unordered_map<PageId, frame_id_t, PageIdHash> page_table_;
        std::vector<frame_id_t> free_list_;
        std::unique_ptr<Replacer> replacer_;
        std::mutex latch_;
        frame_id_t next_unused_frame_;
        frame_id_t frame_limit_;

        Shard(size_t pool_size, frame_id_t first_frame, frame_id_t frame_limit)
            : replacer_(std::make_unique<LRUReplacer>(pool_size)),
              next_unused_frame_(first_frame),
              frame_limit_(frame_limit) {}
    };

    size_t pool_size_;      // buffer_pool中可容纳页面的个数，即帧的个数
    size_t shard_count_;
    Page *pages_ = nullptr; // buffer_pool中的Page对象数组，在构造空间中申请内存空间，在析构函数中释放，大小为BUFFER_POOL_SIZE
    std::vector<uint8_t> frame_mapped_;
    std::vector<uint8_t> frame_constructed_;
    std::vector<std::unique_ptr<Shard>> shards_;
    DiskManager *disk_manager_;

   public:
    BufferPoolManager(size_t pool_size, DiskManager *disk_manager)
        : pool_size_(pool_size), shard_count_(choose_shard_count(pool_size)), disk_manager_(disk_manager) {
        shards_.reserve(shard_count_);
        frame_id_t next_frame = 0;
        for (size_t i = 0; i < shard_count_; ++i) {
            size_t shard_frame_count = pool_size_ / shard_count_ + (i < pool_size_ % shard_count_ ? 1 : 0);
            frame_id_t frame_limit = next_frame + static_cast<frame_id_t>(shard_frame_count);
            shards_.emplace_back(std::make_unique<Shard>(pool_size_, next_frame, frame_limit));
            shards_.back()->free_list_.reserve(shard_frame_count);
            next_frame = frame_limit;
        }
        frame_mapped_.assign(pool_size_, 0);
        frame_constructed_.assign(pool_size_, 0);
        // 为buffer pool保留连续存储空间，Page对象在frame首次使用时再构造。
        pages_ = static_cast<Page*>(::operator new[](sizeof(Page) * pool_size_));
    }

    ~BufferPoolManager() {
        for (size_t i = 0; i < pool_size_; ++i) {
            if (frame_constructed_[i]) {
                pages_[i].~Page();
            }
        }
        ::operator delete[](pages_);
    }

    /**
     * @description: 将目标页面标记为脏页
     * @param {Page*} page 脏页
     */
    static void mark_dirty(Page* page) { page->is_dirty_ = true; }

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

    void update_page(Shard& shard, Page* page, PageId new_page_id, frame_id_t new_frame_id, bool clear_data);
};
