#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <iostream>

int char2int(char input) {
    if(input >= '0' && input <= '9')
        return input - '0';
    if(input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if(input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
std::vector<uint8_t> hex2bin(const std::string & src_str) {
    const char * src = src_str.c_str();
    std::vector<uint8_t> output;
    while(*src && src[1]) {
        output.push_back(char2int(*src)*16 + char2int(src[1]));
        src += 2;
    }
    return output;
}

class TestDataContainer {
  public:
    std::string name;
    std::string hex_str;
    std::string json_str;

    TestDataContainer(const std::string &name, const std::string &hex_str, const std::string &json_str) {
        this->name = name;
        this->hex_str = hex_str;
        this->json_str = json_str;
    }

    friend std::ostream & operator << (std::ostream &out, const TestDataContainer &c);
};


std::ostream & operator << (std::ostream &out, const TestDataContainer &c) {
    out << c.name;
    return out;
}
