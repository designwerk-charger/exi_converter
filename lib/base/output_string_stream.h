#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

class OutputStringStream {
  public:
    OutputStringStream();

    void start_key(const std::string & key);
    void add_value(const std::string & value);
    void add_value(int32_t value);
    void add_value(bool value);
    void end_key();

    void start_list();
    void next_item();
    void end_list();

    std::string get_full_stream();

  private:
    enum commands_t {nocmd, startkey, addvalue, startlist, nextitem, endlist, endkey};
    commands_t last_command_;
    std::ostringstream output_data_;
};
