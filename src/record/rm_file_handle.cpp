/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "rm_file_handle.h"

std::mutex RmFileHandle::shared_states_latch_;
std::unordered_map<std::string, std::weak_ptr<RmFileHandle::SharedRecordState>> RmFileHandle::shared_states_;

std::shared_ptr<RmFileHandle::SharedRecordState> RmFileHandle::get_shared_state(const std::string &file_name) {
    std::lock_guard<std::mutex> guard(shared_states_latch_);
    auto &weak_state = shared_states_[file_name];
    auto state = weak_state.lock();
    if (state == nullptr) {
        state = std::make_shared<SharedRecordState>();
        weak_state = state;
    }
    return state;
}

void RmFileHandle::detach_shared_state() const {
    std::lock_guard<std::mutex> guard(shared_states_latch_);
    auto it = shared_states_.find(file_name_);
    if (it != shared_states_.end() && it->second.lock() == shared_state_) {
        shared_states_.erase(it);
    }
}

bool RmFileHandle::is_record(const Rid &rid) const {
    std::shared_lock<std::shared_mutex> handle_guard(latch_);
    ensure_open_locked();
    std::shared_lock<std::shared_mutex> guard(shared_state_->latch);
    return shared_state_->records.find(memory_key(rid)) != shared_state_->records.end();
}

/**
 * @description: 获取当前表中记录号为rid的记录
 * @param {Rid&} rid 记录号，指定记录的位置
 * @param {Context*} context
 * @return {unique_ptr<RmRecord>} rid对应的记录对象指针
 */
std::unique_ptr<RmRecord> RmFileHandle::get_record(const Rid& rid, Context* context) const {
    std::shared_lock<std::shared_mutex> handle_guard(latch_);
    ensure_open_locked();
    std::shared_lock<std::shared_mutex> guard(shared_state_->latch);
    auto mem_it = shared_state_->records.find(memory_key(rid));
    if (mem_it == shared_state_->records.end()) {
        throw RecordNotFoundError(rid.page_no, rid.slot_no);
    }
    return std::make_unique<RmRecord>(*mem_it->second);
}

/**
 * @description: 在当前表中插入一条记录，不指定插入位置
 * @param {char*} buf 要插入的记录的数据
 * @param {Context*} context
 * @return {Rid} 插入的记录的记录号（位置）
 */
Rid RmFileHandle::insert_record(char* buf, Context* context) {
    std::unique_lock<std::shared_mutex> guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> state_guard(shared_state_->latch);
    file_hdr_ = shared_state_->file_hdr;
    // Todo:
    // 1. 获取当前未满的page handle
    RmPageHandle page_handle = create_page_handle();
    // 2. 在page handle中找到空闲slot位置
    int slot_no = Bitmap::first_bit(false, page_handle.bitmap, file_hdr_.num_records_per_page);
    // 3. 将buf复制到空闲slot位置
    char *slot = page_handle.get_slot(slot_no);
    memcpy(slot, buf, file_hdr_.record_size);
    // 4. 更新page_handle.page_hdr中的数据结构
    Bitmap::set(page_handle.bitmap, slot_no);
    page_handle.page_hdr->num_records++;
    // 注意考虑插入一条记录后页面已满的情况，需要更新file_hdr_.first_free_page_no
    if (page_handle.page_hdr->num_records == file_hdr_.num_records_per_page) {
        remove_page_from_free_list(page_handle);
    }
    page_id_t page_no = page_handle.page->get_page_id().page_no;
    shared_state_->records[memory_key(Rid{page_no, slot_no})] =
        std::make_unique<RmRecord>(file_hdr_.record_size, buf);
    shared_state_->file_hdr = file_hdr_;
    buffer_pool_manager_->mark_dirty(page_handle.page);
    buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);
    return Rid{page_no, slot_no};

}

/**
 * @description: 在当前表中的指定位置插入一条记录
 * @param {Rid&} rid 要插入记录的位置
 * @param {char*} buf 要插入记录的数据
 */
void RmFileHandle::insert_record(const Rid& rid, char* buf) {
    std::unique_lock<std::shared_mutex> guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> state_guard(shared_state_->latch);
    file_hdr_ = shared_state_->file_hdr;
    RmPageHandle page_handle = fetch_page_handle_unlocked(rid.page_no);
    if (rid.slot_no < 0 || rid.slot_no >= file_hdr_.num_records_per_page) {
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        throw RecordNotFoundError(rid.page_no, rid.slot_no);
    }
    if (Bitmap::is_set(page_handle.bitmap, rid.slot_no)) {
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        throw RMDBError("Record already exists: (" + std::to_string(rid.page_no) + "," +
                        std::to_string(rid.slot_no) + ")");
    }
    char *slot = page_handle.get_slot(rid.slot_no);
    memcpy(slot, buf, file_hdr_.record_size);
    Bitmap::set(page_handle.bitmap, rid.slot_no);
    page_handle.page_hdr->num_records++;
    if (page_handle.page_hdr->num_records == file_hdr_.num_records_per_page) {
        remove_page_from_free_list(page_handle);
    }
    shared_state_->records[memory_key(rid)] = std::make_unique<RmRecord>(file_hdr_.record_size, buf);
    shared_state_->file_hdr = file_hdr_;
    buffer_pool_manager_->mark_dirty(page_handle.page);
    buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);
}

/**
 * @description: 删除记录文件中记录号为rid的记录
 * @param {Rid&} rid 要删除的记录的记录号（位置）
 * @param {Context*} context
 */
void RmFileHandle::delete_record(const Rid& rid, Context* context) {
    std::unique_lock<std::shared_mutex> guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> state_guard(shared_state_->latch);
    file_hdr_ = shared_state_->file_hdr;
    // Todo:
    // 1. 获取指定记录所在的page handle
    RmPageHandle page_handle = fetch_page_handle_unlocked(rid.page_no);
    if (!record_exists_on_page(rid, page_handle)) {
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        throw RecordNotFoundError(rid.page_no, rid.slot_no);
    }
    // 2. 更新page_handle.page_hdr中的数据结构
    Bitmap::reset(page_handle.bitmap, rid.slot_no);
    page_handle.page_hdr->num_records--;
    // 注意考虑删除一条记录后页面未满的情况，需要调用release_page_handle()
    if (page_handle.page_hdr->num_records == file_hdr_.num_records_per_page - 1) {
        release_page_handle(page_handle);
    }
    shared_state_->records.erase(memory_key(rid));
    shared_state_->file_hdr = file_hdr_;
    buffer_pool_manager_->mark_dirty(page_handle.page);
    buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);
}


/**
 * @description: 更新记录文件中记录号为rid的记录
 * @param {Rid&} rid 要更新的记录的记录号（位置）
 * @param {char*} buf 新记录的数据
 * @param {Context*} context
 */
void RmFileHandle::update_record(const Rid& rid, char* buf, Context* context) {
    std::unique_lock<std::shared_mutex> guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> state_guard(shared_state_->latch);
    file_hdr_ = shared_state_->file_hdr;
    // Todo:
    // 1. 获取指定记录所在的page handle
    RmPageHandle page_handle = fetch_page_handle_unlocked(rid.page_no);
    if (!record_exists_on_page(rid, page_handle)) {
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        throw RecordNotFoundError(rid.page_no, rid.slot_no);
    }
    // 2. 更新记录
    char *slot = page_handle.get_slot(rid.slot_no);
    memcpy(slot, buf, file_hdr_.record_size);
    auto mem_key = memory_key(rid);
    auto mem_it = shared_state_->records.find(mem_key);
    if (mem_it != shared_state_->records.end()) {
        mem_it->second->SetData(buf);
    } else {
        shared_state_->records[mem_key] = std::make_unique<RmRecord>(file_hdr_.record_size, buf);
    }
    shared_state_->file_hdr = file_hdr_;
    buffer_pool_manager_->mark_dirty(page_handle.page);
    buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);

}

void RmFileHandle::reset_data_pages() {
    std::unique_lock<std::shared_mutex> guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> state_guard(shared_state_->latch);
    file_hdr_ = shared_state_->file_hdr;
    for (int page_no = RM_FIRST_RECORD_PAGE; page_no < file_hdr_.num_pages; ++page_no) {
        buffer_pool_manager_->delete_page(PageId{fd_, page_no});
    }
    file_hdr_.num_pages = RM_FIRST_RECORD_PAGE;
    file_hdr_.first_free_page_no = RM_NO_PAGE;
    disk_manager_->set_fd2pageno(fd_, file_hdr_.num_pages);
    shared_state_->records.clear();
    shared_state_->file_hdr = file_hdr_;
    shared_state_->records_loaded = true;
}

void RmFileHandle::reserve_memory_records(size_t count) {
    std::shared_lock<std::shared_mutex> handle_guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> guard(shared_state_->latch);
    shared_state_->records.reserve(shared_state_->records.size() + count);
}

std::vector<Rid> RmFileHandle::snapshot_rids() const {
    std::shared_lock<std::shared_mutex> handle_guard(latch_);
    ensure_open_locked();
    std::shared_lock<std::shared_mutex> guard(shared_state_->latch);
    std::vector<Rid> rids;
    rids.reserve(shared_state_->records.size());
    for (const auto &entry : shared_state_->records) {
        rids.push_back(key_to_rid(entry.first));
    }
    std::sort(rids.begin(), rids.end(), [](const Rid &lhs, const Rid &rhs) {
        if (lhs.page_no != rhs.page_no) {
            return lhs.page_no < rhs.page_no;
        }
        return lhs.slot_no < rhs.slot_no;
    });
    return rids;
}

/**
 * 以下函数为辅助函数，仅提供参考，可以选择完成如下函数，也可以删除如下函数，在单元测试中不涉及如下函数接口的直接调用
*/
/**
 * @description: 获取指定页面的页面句柄
 * @param {int} page_no 页面号
 * @return {RmPageHandle} 指定页面的句柄
 */
RmPageHandle RmFileHandle::fetch_page_handle(int page_no) const {
    std::shared_lock<std::shared_mutex> handle_guard(latch_);
    ensure_open_locked();
    return fetch_page_handle_unlocked(page_no);
}

RmPageHandle RmFileHandle::fetch_page_handle_unlocked(int page_no) const {
    // Todo:
    // 使用缓冲池获取指定页面，并生成page_handle返回给上层
    // if page_no is invalid, throw PageNotExistError exception
    if (page_no < RM_FIRST_RECORD_PAGE || page_no >= file_hdr_.num_pages) {
        throw PageNotExistError(disk_manager_->get_file_name(fd_), page_no);
    }
    Page *page = buffer_pool_manager_->fetch_page(PageId{fd_, page_no});
    return RmPageHandle(&file_hdr_, page);
}

/**
 * @description: 创建一个新的page handle
 * @return {RmPageHandle} 新的PageHandle
 */
RmPageHandle RmFileHandle::create_new_page_handle() {
    std::unique_lock<std::shared_mutex> guard(latch_);
    ensure_open_locked();
    std::unique_lock<std::shared_mutex> state_guard(shared_state_->latch);
    file_hdr_ = shared_state_->file_hdr;
    auto page_handle = create_new_page_handle_unlocked();
    shared_state_->file_hdr = file_hdr_;
    return page_handle;
}

RmPageHandle RmFileHandle::create_new_page_handle_unlocked() {
    // Todo:
    // 1.使用缓冲池来创建一个新page
    // new_page会在fd对应的文件中分配新的page_no（从file_hdr_.num_pages开始递增），
    // 并将分配结果写回page_id，避免与disk_manager的页号分配重复
    PageId page_id{fd_, file_hdr_.num_pages};
    Page *page = buffer_pool_manager_->new_page_at(page_id);
    if (page == nullptr) {
        throw InternalError("BufferPoolManager::new_page_at failed");
    }
    page_id_t page_no = page_id.page_no;
    disk_manager_->set_fd2pageno(fd_, page_no + 1);
    file_hdr_.num_pages++;
    // 2.更新page handle中的相关信息
    RmPageHandle page_handle(&file_hdr_, page);
    page_handle.page_hdr->next_free_page_no = file_hdr_.first_free_page_no;
    page_handle.page_hdr->num_records = 0;
    Bitmap::init(page_handle.bitmap, file_hdr_.bitmap_size);
    // 3.更新file_hdr_
    file_hdr_.first_free_page_no = page_no;
    return page_handle;
}

/**
 * @brief 创建或获取一个空闲的page handle
 *
 * @return RmPageHandle 返回生成的空闲page handle
 * @note pin the page, remember to unpin it outside!
 */
RmPageHandle RmFileHandle::create_page_handle() {
    // Todo:
    // 1. 判断file_hdr_中是否还有空闲页
    //     1.1 没有空闲页：使用缓冲池来创建一个新page；可直接调用create_new_page_handle()
    //     1.2 有空闲页：直接获取第一个空闲页
    // 2. 生成page handle并返回给上层
    if (file_hdr_.first_free_page_no == RM_NO_PAGE) {
        return create_new_page_handle_unlocked();
    }
    return fetch_page_handle_unlocked(file_hdr_.first_free_page_no);
}

/**
 * @description: 当一个页面从没有空闲空间的状态变为有空闲空间状态时，更新文件头和页头中空闲页面相关的元数据
 */
void RmFileHandle::release_page_handle(RmPageHandle&page_handle) {
    // Todo:
    // 当page从已满变成未满，考虑如何更新：
    // 1. page_handle.page_hdr->next_free_page_no
    page_handle.page_hdr->next_free_page_no = file_hdr_.first_free_page_no;
    // 2. file_hdr_.first_free_page_no
    file_hdr_.first_free_page_no = page_handle.page->get_page_id().page_no;
}

void RmFileHandle::load_memory_records() {
    std::unique_lock<std::shared_mutex> guard(shared_state_->latch);
    if (shared_state_->records_loaded) {
        file_hdr_ = shared_state_->file_hdr;
        return;
    }
    shared_state_->records.clear();
    int file_pages = disk_manager_->get_file_size(disk_manager_->get_file_name(fd_)) / PAGE_SIZE;
    int loaded_pages = std::min(file_hdr_.num_pages, file_pages);
    for (int page_no = RM_FIRST_RECORD_PAGE; page_no < loaded_pages; ++page_no) {
        Page *page = buffer_pool_manager_->fetch_page(PageId{fd_, page_no});
        RmPageHandle page_handle(&file_hdr_, page);
        for (int slot_no = 0; slot_no < file_hdr_.num_records_per_page; ++slot_no) {
            if (Bitmap::is_set(page_handle.bitmap, slot_no)) {
                shared_state_->records[memory_key(Rid{page_no, slot_no})] =
                    std::make_unique<RmRecord>(file_hdr_.record_size, page_handle.get_slot(slot_no));
            }
        }
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
    }
    shared_state_->records_loaded = true;
}

void RmFileHandle::remove_page_from_free_list(RmPageHandle &page_handle) {
    int page_no = page_handle.page->get_page_id().page_no;
    int next_free_page_no = page_handle.page_hdr->next_free_page_no;
    if (file_hdr_.first_free_page_no == page_no) {
        file_hdr_.first_free_page_no = next_free_page_no;
        page_handle.page_hdr->next_free_page_no = RM_NO_PAGE;
        return;
    }

    int prev_page_no = file_hdr_.first_free_page_no;
    while (prev_page_no != RM_NO_PAGE) {
        RmPageHandle prev_page_handle = fetch_page_handle_unlocked(prev_page_no);
        int prev_next_page_no = prev_page_handle.page_hdr->next_free_page_no;
        if (prev_next_page_no == page_no) {
            prev_page_handle.page_hdr->next_free_page_no = next_free_page_no;
            buffer_pool_manager_->mark_dirty(prev_page_handle.page);
            buffer_pool_manager_->unpin_page(prev_page_handle.page->get_page_id(), true);
            page_handle.page_hdr->next_free_page_no = RM_NO_PAGE;
            return;
        }
        buffer_pool_manager_->unpin_page(prev_page_handle.page->get_page_id(), false);
        prev_page_no = prev_next_page_no;
    }
    page_handle.page_hdr->next_free_page_no = RM_NO_PAGE;
}

bool RmFileHandle::record_exists_on_page(const Rid &rid, const RmPageHandle &page_handle) const {
    return rid.slot_no >= 0 && rid.slot_no < file_hdr_.num_records_per_page &&
           Bitmap::is_set(page_handle.bitmap, rid.slot_no);
}
