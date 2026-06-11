#pragma once

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "execution_defs.h"
#include "executor_abstract.h"
#include "record/rm.h"

class AggregateExecutor : public AbstractExecutor {
   private:
    struct AggState {
        int count = 0;
        double sum = 0;
        bool has_value = false;
        int int_val = 0;
        float float_val = 0;
    };

    struct GroupState {
        std::unique_ptr<RmRecord> first_rec;
        std::vector<AggState> aggs;
    };

    std::unique_ptr<AbstractExecutor> prev_;
    std::vector<SelectTerm> select_terms_;
    std::vector<AggCall> agg_calls_;
    std::vector<TabCol> group_cols_;
    std::vector<AggHavingCond> having_conds_;
    std::vector<ColMeta> cols_;
    size_t len_ = 0;
    std::vector<std::unique_ptr<RmRecord>> results_;
    size_t pos_ = 0;

    std::string group_key(const RmRecord &rec) const {
        std::string key;
        for (auto &group_col : group_cols_) {
            auto meta = find_col(prev_->cols(), group_col);
            key.append(rec.data + meta->offset, meta->len);
            key.push_back('\0');
        }
        return key;
    }

    std::vector<ColMeta>::const_iterator find_col(const std::vector<ColMeta> &rec_cols, const TabCol &target) const {
        auto pos = std::find_if(rec_cols.begin(), rec_cols.end(), [&](const ColMeta &col) {
            return col.tab_name == target.tab_name && col.name == target.col_name;
        });
        if (pos == rec_cols.end()) {
            throw ColumnNotFoundError(target.tab_name + '.' + target.col_name);
        }
        return pos;
    }

    void update_agg(AggState &state, const AggCall &call, const RmRecord &rec) {
        state.count++;
        if (call.type == AGG_COUNT) return;

        auto meta = find_col(prev_->cols(), call.col);
        if (meta->type == TYPE_INT) {
            int v = *(int *)(rec.data + meta->offset);
            state.sum += v;
            if (!state.has_value ||
                ((call.type == AGG_MAX) && v > state.int_val) ||
                ((call.type == AGG_MIN) && v < state.int_val)) {
                state.int_val = v;
            }
        } else {
            float v = *(float *)(rec.data + meta->offset);
            state.sum += v;
            if (!state.has_value ||
                ((call.type == AGG_MAX) && v > state.float_val) ||
                ((call.type == AGG_MIN) && v < state.float_val)) {
                state.float_val = v;
            }
        }
        state.has_value = true;
    }

    Value agg_value(const AggCall &call, const AggState &state) const {
        Value v;
        if (call.type == AGG_COUNT) {
            v.set_int(state.count);
        } else if (call.type == AGG_AVG) {
            v.set_float(state.count == 0 ? 0 : (float)(state.sum / state.count));
        } else if (call.type == AGG_SUM) {
            if (call.result_type == TYPE_INT) v.set_int((int)state.sum);
            else v.set_float((float)state.sum);
        } else if (call.result_type == TYPE_INT) {
            v.set_int(state.has_value ? state.int_val : 0);
        } else {
            v.set_float(state.has_value ? state.float_val : 0);
        }
        return v;
    }

    Value col_value(const TabCol &tc, const GroupState &group) const {
        auto meta = find_col(prev_->cols(), tc);
        Value v;
        char *data = group.first_rec->data + meta->offset;
        if (meta->type == TYPE_INT) {
            v.set_int(*(int *)data);
        } else if (meta->type == TYPE_FLOAT) {
            v.set_float(*(float *)data);
        } else {
            std::string s(data, meta->len);
            s.resize(strlen(s.c_str()));
            v.set_str(s);
        }
        return v;
    }

    Value term_value(const AggTerm &term, const GroupState &group) const {
        if (term.kind == AGG_TERM_AGG) {
            return agg_value(agg_calls_[term.agg_idx], group.aggs[term.agg_idx]);
        }
        if (term.kind == AGG_TERM_COL) {
            return col_value(term.col, group);
        }
        return term.val;
    }

    bool compare_values(const Value &lhs, CompOp op, const Value &rhs) const {
        bool numeric = (lhs.type == TYPE_INT || lhs.type == TYPE_FLOAT) &&
                       (rhs.type == TYPE_INT || rhs.type == TYPE_FLOAT);
        int cmp = 0;
        if (numeric) {
            double lv = lhs.type == TYPE_INT ? lhs.int_val : lhs.float_val;
            double rv = rhs.type == TYPE_INT ? rhs.int_val : rhs.float_val;
            cmp = (lv > rv) - (lv < rv);
        } else {
            cmp = (lhs.str_val > rhs.str_val) - (lhs.str_val < rhs.str_val);
        }
        switch (op) {
            case OP_EQ: return cmp == 0;
            case OP_NE: return cmp != 0;
            case OP_LT: return cmp < 0;
            case OP_GT: return cmp > 0;
            case OP_LE: return cmp <= 0;
            case OP_GE: return cmp >= 0;
        }
        return false;
    }

    bool pass_having(const GroupState &group) const {
        for (auto &cond : having_conds_) {
            if (!compare_values(term_value(cond.lhs, group), cond.op, term_value(cond.rhs, group))) {
                return false;
            }
        }
        return true;
    }

    void write_value(RmRecord &rec, int offset, ColType type, int len, const Value &v) {
        if (type == TYPE_INT) {
            *(int *)(rec.data + offset) = v.type == TYPE_FLOAT ? (int)v.float_val : v.int_val;
        } else if (type == TYPE_FLOAT) {
            *(float *)(rec.data + offset) = v.type == TYPE_INT ? (float)v.int_val : v.float_val;
        } else {
            memset(rec.data + offset, 0, len);
            memcpy(rec.data + offset, v.str_val.c_str(), std::min((int)v.str_val.size(), len));
        }
    }

    std::unique_ptr<RmRecord> build_record(const GroupState &group) {
        auto rec = std::make_unique<RmRecord>((int)len_);
        for (size_t i = 0; i < select_terms_.size(); i++) {
            const auto &term = select_terms_[i];
            Value v;
            if (term.is_agg) {
                v = agg_value(agg_calls_[term.agg_idx], group.aggs[term.agg_idx]);
            } else {
                v = col_value(term.col, group);
            }
            write_value(*rec, cols_[i].offset, cols_[i].type, cols_[i].len, v);
        }
        return rec;
    }

   public:
    AggregateExecutor(std::unique_ptr<AbstractExecutor> prev, std::vector<SelectTerm> select_terms,
                      std::vector<AggCall> agg_calls, std::vector<TabCol> group_cols,
                      std::vector<AggHavingCond> having_conds) {
        prev_ = std::move(prev);
        select_terms_ = std::move(select_terms);
        agg_calls_ = std::move(agg_calls);
        group_cols_ = std::move(group_cols);
        having_conds_ = std::move(having_conds);

        int offset = 0;
        for (auto &term : select_terms_) {
            ColMeta col;
            col.tab_name = "__agg";
            col.name = term.output_name;
            col.type = term.type;
            col.len = term.len;
            col.offset = offset;
            col.index = false;
            offset += col.len;
            cols_.push_back(col);
        }
        len_ = offset;
    }

    void beginTuple() override {
        results_.clear();
        std::vector<GroupState> groups;
        std::map<std::string, size_t> group_pos;

        prev_->beginTuple();
        while (!prev_->is_end()) {
            auto rec = prev_->Next();
            if (rec != nullptr) {
                std::string key = group_key(*rec);
                auto it = group_pos.find(key);
                if (it == group_pos.end()) {
                    GroupState group;
                    group.first_rec = std::make_unique<RmRecord>(*rec);
                    group.aggs.resize(agg_calls_.size());
                    groups.push_back(std::move(group));
                    it = group_pos.emplace(key, groups.size() - 1).first;
                }
                auto &group = groups[it->second];
                for (size_t i = 0; i < agg_calls_.size(); i++) {
                    update_agg(group.aggs[i], agg_calls_[i], *rec);
                }
            }
            prev_->nextTuple();
        }

        if (groups.empty() && group_cols_.empty()) {
            GroupState group;
            group.first_rec = std::make_unique<RmRecord>(prev_->tupleLen());
            memset(group.first_rec->data, 0, prev_->tupleLen());
            group.aggs.resize(agg_calls_.size());
            groups.push_back(std::move(group));
        }

        for (auto &group : groups) {
            if (pass_having(group)) {
                results_.push_back(build_record(group));
            }
        }
        pos_ = 0;
    }

    void nextTuple() override {
        if (pos_ < results_.size()) pos_++;
    }

    bool is_end() const override { return pos_ >= results_.size(); }

    std::unique_ptr<RmRecord> Next() override {
        if (is_end()) return nullptr;
        rows_++;
        return std::make_unique<RmRecord>(*results_[pos_]);
    }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const override { return len_; }

    Rid &rid() override { return _abstract_rid; }
};
