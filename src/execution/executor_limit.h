#pragma once

#include <memory>

#include "executor_abstract.h"

class LimitExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;
    int limit_ = -1;
    int emitted_ = 0;

   public:
    LimitExecutor(std::unique_ptr<AbstractExecutor> prev, int limit) {
        prev_ = std::move(prev);
        limit_ = limit;
    }

    void beginTuple() override {
        emitted_ = 0;
        prev_->beginTuple();
    }

    void nextTuple() override {
        if (emitted_ < limit_) prev_->nextTuple();
    }

    bool is_end() const override {
        return emitted_ >= limit_ || prev_->is_end();
    }

    void finish() override { prev_->finish(); }

    std::unique_ptr<RmRecord> Next() override {
        if (is_end()) return nullptr;
        emitted_++;
        rows_++;
        return prev_->Next();
    }

    size_t NextBatch(std::vector<std::unique_ptr<RmRecord>> &batch, size_t max_batch_size) override {
        batch.clear();
        if (is_end()) {
            return 0;
        }
        size_t remaining = static_cast<size_t>(limit_ - emitted_);
        size_t n = prev_->NextBatch(batch, std::min(max_batch_size, remaining));
        emitted_ += static_cast<int>(n);
        rows_ += n;
        return n;
    }

    const std::vector<ColMeta> &cols() const override { return prev_->cols(); }

    size_t tupleLen() const override { return prev_->tupleLen(); }

    Rid &rid() override { return prev_->rid(); }
};
