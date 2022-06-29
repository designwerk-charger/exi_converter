#include <vector>
#include <iostream>
#include "input_string_stream.h"


InputStringStream::InputStringStream(const std::string & input_data) :
    input_data_(input_data), item_cnt_(0) {
    std::size_t prev = 0, pos;
    while ((pos = input_data_.find_first_of(":{},\"", prev)) != std::string::npos) {
        if (pos > prev) {
            auto s_str = input_data_.substr(prev, pos - prev);
            if (s_str != " ") {
                input_items_.push_back(s_str);
            }
        }
        prev = pos+1;
    }
}

std::string InputStringStream::get_next_item() {
    return input_items_.at(item_cnt_++);
}
