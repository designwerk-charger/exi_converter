#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

class InputStringStream {
  public:
    InputStringStream(const std::string & input_data);

    std::string get_item_and_move_to_next();
    std::string get_item();

    std::string get_input_data() {return input_data_;};

  private:
    std::string input_data_;
    std::vector<std::string> input_items_;
    uint8_t item_cnt_;
};
