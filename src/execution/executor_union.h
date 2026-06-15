#pragma once

#include <algorithm>
#include <cstring>
#include <memory>
#include <unordered_set>
#include <vector>

#include "executor_abstract.h"

class UnionExecutor : public AbstractExecutor {
   private:
    std::vector<std::unique_ptr<AbstractExecutor>> children_;
    std::vector<ColMeta> cols_;
    std::vector<std::unique_ptr<RmRecord>> tuples_;
    size_t len_ = 0;
    size_t pos_ = 0;

    static std::string read_string(const char *data, int len) {
        std::string s(data, len);
        s.resize(strlen(s.c_str()));
        return s;
    }

    static void write_value(char *dst, const ColMeta &dst_col, const char *src, const ColMeta &src_col) {
        if (dst_col.type == TYPE_FLOAT && src_col.type == TYPE_INT) {
            float v = (float)(*(const int *)src);
            memcpy(dst, &v, sizeof(float));
            return;
        }
        if (dst_col.type == TYPE_STRING && src_col.type == TYPE_STRING) {
            memset(dst, 0, dst_col.len);
            std::string s = read_string(src, src_col.len);
            memcpy(dst, s.data(), std::min((int)s.size(), dst_col.len));
            return;
        }
        memcpy(dst, src, dst_col.len);
    }

   public:
    UnionExecutor(std::vector<std::unique_ptr<AbstractExecutor>> children, std::vector<ColMeta> cols) {
        children_ = std::move(children);
        cols_ = std::move(cols);
        if (!cols_.empty()) {
            len_ = cols_.back().offset + cols_.back().len;
        }
    }

    void beginTuple() override {
        tuples_.clear();
        pos_ = 0;
        std::unordered_set<std::string> seen;
        for (auto &child : children_) {
            child->beginTuple();
            while (!child->is_end()) {
                auto rec = child->Next();
                if (rec != nullptr) {
                    auto out = std::make_unique<RmRecord>(len_);
                    memset(out->data, 0, len_);
                    const auto &src_cols = child->cols();
                    for (size_t i = 0; i < cols_.size(); i++) {
                        const ColMeta &dst_col = cols_[i];
                        const ColMeta &src_col = src_cols[i];
                        write_value(out->data + dst_col.offset, dst_col, rec->data + src_col.offset, src_col);
                    }
                    std::string key(out->data, len_);
                    if (seen.insert(key).second) {
                        tuples_.push_back(std::move(out));
                    }
                }
                child->nextTuple();
            }
        }
    }

    void nextTuple() override {
        if (pos_ < tuples_.size()) pos_++;
    }

    bool is_end() const override { return pos_ >= tuples_.size(); }

    void finish() override {
        for (auto &child : children_) {
            child->finish();
        }
    }

    std::unique_ptr<RmRecord> Next() override {
        if (is_end()) return nullptr;
        rows_++;
        return std::make_unique<RmRecord>(*tuples_[pos_]);
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return _abstract_rid; }
};
