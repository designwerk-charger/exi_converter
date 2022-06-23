#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <iostream>


int char2int(char input);

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
std::vector<uint8_t> hex2bin(const std::string & src_str);

class TestDataContainer {
  public:
    std::string name;
    std::string hex_str;
    std::string json_str;

    TestDataContainer(const std::string &name, const std::string &hex_str, const std::string &json_str);

    friend std::ostream & operator << (std::ostream &out, const TestDataContainer &c);
};

std::string run_decoding(const TestDataContainer & data, const std::string & ns);
