#include "library.h"


int main(int argc, char *argv[]) {

    if (argc != 4) {

    }
    ExiCodec tmp;

    char* byte_stream = (char*)"BYTE_STREAM_AS_STRING";

    tmp.decode(byte_stream, "namespace");
    tmp.encode("json", "namespace");

    // hello();
}