#include <vector>
#include "stringstream.h"


StringStream::StringStream(const std::string & input_data) :
    input_data_(input_data), output_data_(), item_cnt_(0) {
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

std::string StringStream::get_next_item(void) {
    return input_items_.at(item_cnt_++);
}

void StringStream::start_key(const std::string & key) { }
void StringStream::end_key() { }
void StringStream::add_value(const std::string & value) { }
void StringStream::add_value(int32_t value) { }
