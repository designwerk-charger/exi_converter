#include "type_conversion.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "library.h"

int char2int(char input) {
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
std::vector<uint8_t> hex2bin(const std::string & src_str) {
    const char * src = src_str.c_str();
    std::vector<uint8_t> output;
    while (*src && src[1]) {
        output.push_back(char2int(*src)*16 + char2int(src[1]));
        src += 2;
    }
    return output;
}

std::string bin2hex(const std::vector<uint8_t> & src_data) {
    std::ostringstream out_str;
    for (const auto & data : src_data) {
        out_str << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(data);
    }
    return out_str.str();
}

TestDataContainer::TestDataContainer(const std::string &name, const std::string &hex_str, const std::string &json_str) {
    this->name = name;
    this->hex_str = hex_str;
    this->json_str = json_str;
}

std::ostream & operator << (std::ostream &out, const TestDataContainer &c) {
    out << c.name;
    return out;
}

std::string run_decoding(const TestDataContainer & data, const std::string & ns) {
    ExiCodec codec;

    auto input_data = hex2bin(data.hex_str);
    std::string output_data;

    try {
        output_data = codec.decode(input_data, ns);
    } catch (const std::exception& ex) {
        std::cout << "Failed decoding message " << data.name << " (" << ex.what() << ")" << std::endl;
    }
    std::cout << "Result:   " << output_data << std::endl;
    std::cout << "Expected: " << data.json_str << std::endl;
    return output_data;
}

std::string run_encoding(const TestDataContainer & data, const std::string & ns) {
    ExiCodec codec;
    std::string output_str;

    try {
        auto output_data = codec.encode(data.json_str, ns);
        output_str = bin2hex(output_data);
    } catch (const std::exception& ex) {
        std::cout << "Failed encoding message " << data.name << " (" << ex.what() << ")" << std::endl;
    }

    std::cout << "Encoding " << data.json_str << std::endl;
    std::cout << "Result:   " << output_str << std::endl;
    std::cout << "Expected: " << data.hex_str << std::endl;
    return output_str;
}
