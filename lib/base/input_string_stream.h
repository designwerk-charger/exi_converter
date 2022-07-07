#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

class InputStringStream {
  public:
    typedef enum {start, ctrl, key_str, value_str, value_int, arr} state_t;
    typedef std::vector<std::pair<std::string, state_t>> item_store_t;

    explicit InputStringStream(const std::string & input_data);
    InputStringStream();
    virtual ~InputStringStream() = default;  // LCOV_EXCL_LINE

    static item_store_t split_items(const std::string & input_data);
    static item_store_t old_split_items(const std::string & input_data);

    virtual void verify_item_and_move_to_next(const std::string & expected_item);
    virtual std::string get_item_and_move_to_next();
    virtual std::string get_item();

    virtual std::string get_input_data() {return input_data_;};

    virtual bool is_list_start_move_to_next();
    virtual bool is_list_end_move_to_next();

  private:
    std::string input_data_;
    item_store_t input_items_;
    uint8_t item_cnt_;
};
