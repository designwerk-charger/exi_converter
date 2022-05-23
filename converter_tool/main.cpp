#include "library.h"

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
void hex2bin(const char* src, uint8_t * target) {
    while(*src && src[1]) {
        *(target++) = char2int(*src)*16 + char2int(src[1]);
        src += 2;
    }
}

void printUsage() {
    std::cout << "Usage:\n" <<
                 "\tdecode <hexstring> <namespace>\n" <<
                 "\tencode <jsonstring> <namespace>\n" << std::endl;
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        std::cerr << "Unexpected number of arguments!\n" << std::endl;
        printUsage();
        return -1;
    }

    ExiCodec tmp;

    if (std::string(argv[1]) == "decode") {
        std::string hexstr(argv[2]);
        int32_t length = hexstr.size() >> 1;
        uint8_t * bindata = static_cast<uint8_t *>(malloc(length));
        hex2bin(hexstr.c_str(), bindata);

        tmp.decode(bindata, length, std::string(argv[3]));
        return 0;
    } else if (std::string(argv[1]) == "encode") {
        char* byte_stream = (char*)"BYTE_STREAM_AS_STRING";
        tmp.encode("json", "namespace");
        return 0;
    }

    std::cerr << "The first argument was neither 'encode' nor 'decode'!" << std::endl;
    printUsage();
    return -1;
}