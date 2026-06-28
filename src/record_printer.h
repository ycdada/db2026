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

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include "common/context.h"
#include "common/config.h"

#define RECORD_COUNT_LENGTH 40

class RecordPrinter {
    static constexpr size_t COL_WIDTH = 16;
    size_t num_cols;
    std::string separator_cell_;

    static void append(Context *context, const char *data, size_t len) {
        if (context->ellipsis_ == false && *context->offset_ + RECORD_COUNT_LENGTH + len < BUFFER_LENGTH) {
            memcpy(context->data_send_ + *(context->offset_), data, len);
            *(context->offset_) += len;
        } else {
            context->ellipsis_ = true;
        }
    }

    static void append_spaces(Context *context, size_t count) {
        static constexpr char SPACES[] = "                ";
        while (count > 0) {
            size_t chunk = count > COL_WIDTH ? COL_WIDTH : count;
            append(context, SPACES, chunk);
            count -= chunk;
        }
    }

public:
    RecordPrinter(size_t num_cols_) : num_cols(num_cols_), separator_cell_("+" + std::string(COL_WIDTH + 2, '-')) {
        assert(num_cols_ > 0);
    }

    void print_separator(Context *context) const {
        for (size_t i = 0; i < num_cols; i++) {
            append(context, separator_cell_.data(), separator_cell_.size());
        }
        append(context, "+\n", 2);
    }

    void print_cell(std::string_view col, Context *context) const {
        append(context, "| ", 2);
        if (col.size() > COL_WIDTH) {
            append(context, col.data(), COL_WIDTH - 3);
            append(context, "...", 3);
        } else {
            append_spaces(context, COL_WIDTH - col.size());
            append(context, col.data(), col.size());
        }
        append(context, " ", 1);
    }

    void finish_record(Context *context) const {
        append(context, "|\n", 2);
    }

    void print_record(const std::vector<std::string> &rec_str, Context *context) const {
        assert(rec_str.size() == num_cols);
        for (const auto &col : rec_str) {
            print_cell(col, context);
        }
        finish_record(context);
    }

    static void print_record_count(size_t num_rec, Context *context) {
        // std::cout << "Total record(s): " << num_rec << '\n';
        std::string str = "";
        if(context->ellipsis_ == true) {
            str = "... ...\n";
        }
        str += "Total record(s): " + std::to_string(num_rec) + '\n';
        memcpy(context->data_send_ + *(context->offset_), str.c_str(), str.length());
        *(context->offset_) = *(context->offset_) + str.length();
    }
};
