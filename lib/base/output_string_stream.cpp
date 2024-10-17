#include <vector>
#include <iostream>
#include "output_string_stream.h"


OutputStringStream::OutputStringStream() :
    output_data_(), last_command_(nocmd) { }

void OutputStringStream::start_key(const std::string & key) {
    if (last_command_ == endkey) {
        output_data_ << ",\"" << key << "\":";
    } else {
        output_data_ << "{\"" << key << "\":";
    }
    last_command_ = startkey;
}

void OutputStringStream::end_key() {
    if (last_command_ == endkey) {
        output_data_ << "}";
    } else if (last_command_ == startkey) {
        output_data_ << "{}";
    }
    last_command_ = endkey;

#ifndef NDEBUG
    std::cout << "->end_key added " << output_data_.str() << std::endl;
#endif
}

void OutputStringStream::add_value(const std::string & value) {
    output_data_ << "\"" << value << "\"";
    last_command_ = addvalue;
}

void OutputStringStream::add_value(int32_t value) {
    output_data_ << value;
    last_command_ = addvalue;
}

void OutputStringStream::add_value(int64_t value) {
    output_data_ << value;
    last_command_ = addvalue;
}

void OutputStringStream::add_value(bool value) {
    output_data_ << ((value == 0) ? "false" : "true");
    last_command_ = addvalue;
}

void OutputStringStream::start_list() {
    if (last_command_ != startkey) {
        throw std::runtime_error("Start list requires preceding start key! (" + get_full_stream() + ")");
    }
    output_data_ << "[";
    last_command_ = startlist;
}

void OutputStringStream::next_item() {
    if (last_command_ == endkey) {
        output_data_ << "},";
    } else if (last_command_ == addvalue) {
        output_data_ << ",";
    }
    last_command_ = nextitem;
}

void OutputStringStream::end_list() {
    if (last_command_ == endkey) {
        output_data_ << "}";
    }
    output_data_ << "]";
    last_command_ = endlist;
}

std::string OutputStringStream::get_full_stream() {
    return output_data_.str() + "}";
}
