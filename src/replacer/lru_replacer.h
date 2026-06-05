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

#include <list>
#include <mutex>  
#include <vector>

#include "common/config.h"
#include "replacer/replacer.h"
#include "unordered_map"

/*
LRUReplacer实现了LRU替换策略
*/
class LRUReplacer : public Replacer {
   public:
    /**
     * @description: 创建一个新的LRUReplacer
     * @param {size_t} num_pages LRUReplacer最多需要存储的page数量
     */
    explicit LRUReplacer(size_t num_pages);

    explicit LRUReplacer();

    ~LRUReplacer();

    bool victim(frame_id_t *frame_id);

    void pin(frame_id_t frame_id);

    void unpin(frame_id_t frame_id);

    size_t Size();

   private:
    std::mutex latch_;                  // 互斥锁
    std::list<frame_id_t> LRUlist_;     // 按加入的时间顺序存放unpinned pages的frame id，首部表示最近被访问
    std::unordered_map<frame_id_t, std::list<frame_id_t>::iterator> LRUhash_;   // frame_id_t -> unpinned pages的frame id
    size_t max_size_;   // 最大容量（与缓冲池的容量相同）


    bool is_pinned_[BUFFER_POOL_SIZE];                            // 哈希表的使用标记，避免重复插入

    // 新增：按实际缓冲池大小动态分配的pin标记，避免pool_size_大于BUFFER_POOL_SIZE时
    // 对固定数组is_pinned_的越界访问（BigStorage等大缓冲池场景）。需要时自动扩容。
    std::vector<bool> pin_flags_;

    // 确保pin_flags_至少能容纳frame_id，新槽位默认标记为已固定(true)
    inline void ensure_capacity(frame_id_t frame_id) {
        if (frame_id >= static_cast<frame_id_t>(pin_flags_.size())) {
            pin_flags_.resize(frame_id + 1, true);
        }
    }
};
