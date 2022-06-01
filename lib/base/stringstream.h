#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

class StringStream {
  public:
    StringStream(const std::string & input_data);

    std::string get_next_item();

    void start_key(const std::string & key);
    void add_value(const std::string & value);
    void add_value(int32_t value);
    void end_key();

    void start_list();
    void next_item();
    void end_list();

    std::string get_full_stream();

  private:
    enum commands_t {nocmd, startkey, addvalue, startlist, nextitem, endlist, endkey};
    commands_t last_command_;
    std::ostringstream output_data_;
    std::string input_data_;
    std::vector<std::string> input_items_;
    uint8_t item_cnt_;
};
