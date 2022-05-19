#!/usr/bin/python3
import argparse
import os


class CmdLineParser:

    def __init__(self):
        self.parser = self.create_parser()

    @staticmethod
    def create_parser() -> argparse.ArgumentParser:
        parser = argparse.ArgumentParser(
            prog="exi_code_generator",
            description='Code Generator for Designwerk EXI Converter Library',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)

        parser.add_argument('--output_path',  help='Directory path to where the generated files will be placed.')

        return parser

    def get_args(self):
        return self.parser.parse_args()


if __name__ == '__main__':
    args = CmdLineParser().get_args()

    with open(os.path.join(args.output_path, "gen_test.cpp"), 'w') as f:
        f.write('#include "gen_test.h"\n'
                'void GenTest::dummy(void) {}\n')

    with open(os.path.join(args.output_path, "gen_test.h"), 'w') as f:
        f.write("#pragma once\n\n"
                "class GenTest {\n"
                "public:\n"
                "void dummy(void);\n"
                "};\n")

    print(args.output_path)
