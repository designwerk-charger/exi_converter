#!/usr/bin/python3

import sys

sys.path.append("../cmake-build-debug")

import exi_converter


print(f"Exi codec version: {exi_converter.__version__}")

codec = exi_converter.ExiCodec()

print(codec.decode(b'abc', "ns"))
print(codec.encode("bla", "ns"))
