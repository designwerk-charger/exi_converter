#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

class InputStringStream {
  public:
    InputStringStream(const std::string & input_data);

    std::string get_next_item();

  private:
    std::string input_data_;
    std::vector<std::string> input_items_;
    uint8_t item_cnt_;
};
