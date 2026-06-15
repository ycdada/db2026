#pragma once

#include <memory>
#include <vector>

#include "executor_abstract.h"

class RenameExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;
    std::vector<ColMeta> cols_;
    size_t len_ = 0;

   public:
    RenameExecutor(std::unique_ptr<AbstractExecutor> prev, std::vector<ColMeta> cols) {
        prev_ = std::move(prev);
        cols_ = std::move(cols);
        len_ = prev_->tupleLen();
    }

    void beginTuple() override { prev_->beginTuple(); }

    void nextTuple() override { prev_->nextTuple(); }

    bool is_end() const override { return prev_->is_end(); }

    void finish() override { prev_->finish(); }

    std::unique_ptr<RmRecord> Next() override {
        auto rec = prev_->Next();
        if (rec != nullptr) rows_++;
        return rec;
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return prev_->rid(); }

    bool bind_join_key(const RmRecord &left_rec, const std::vector<ColMeta> &left_cols) override {
        return prev_->bind_join_key(left_rec, left_cols);
    }
};
