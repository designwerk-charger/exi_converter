#!/usr/bin/python3
import argparse
import os

from dom.base_types import BaseTypes
from dom.body_message import BodyMessage
from dom.complex_types import ComplexTypes
from dom.enums import Enums
from dom.type_tree import TypeTree


class CmdLineParser:

    def __init__(self):
        self.parser = self.create_parser()

    @staticmethod
    def create_parser() -> argparse.ArgumentParser:
        parser = argparse.ArgumentParser(
            prog="exi_code_generator",
            description='Code Generator for Designwerk EXI Converter Library',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)

        parser.add_argument('--output_path', required=True,
                            help='Directory path to where the generated files will be placed.')

        parser.add_argument('--schema_path', required=True,
                            help='Directory where the schema files are located.')

        parser.add_argument('--namespace', required=True,
                            help='Cpp Namespace for generated classes')

        return parser

    def get_args(self):
        return self.parser.parse_args()


if __name__ == '__main__':
    args = CmdLineParser().get_args()
    if len(args.namespace) < 3:
        raise RuntimeError(f"Namespace not correctly set: {args.namespace}")

    tt = TypeTree(schema_file=os.path.join(args.schema_path, 'iso15118_2/V2G_CI_MsgDef.xsd'))

    # write a header file describing basic types which are used by generated code
    basic_types = BaseTypes(tt.type_tree)
    basic_types.write_base_type_header(args.output_path)

    # generate classes for encoding and decoding all enumerations used
    enums = Enums(tt.type_tree, namespace=args.namespace)
    enums.write_enum_conversion_header(args.output_path)
    enums.write_enum_conversion_source(args.output_path)

    # generate classes for encoding and decoding complex types
    ct = ComplexTypes(tt.type_tree, namespace=args.namespace)
    ct.write_complex_type_conversion_header(args.output_path)
    ct.write_complex_type_conversion_source(args.output_path)

    bm = BodyMessage(tt.schema, ct, namespace=args.namespace)
    bm.write_body_conversion_header(args.output_path)
    bm.write_body_conversion_source(args.output_path)

    print(f"Code generated from {args.schema_path} to {args.output_path} with namespace {args.namespace}")