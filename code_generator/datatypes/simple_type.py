import abc
from math import floor, log2, ceil
from typing import List

from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType


class SimpleType(BaseType):

    def __init__(self, type_name: str, type_namespace: str):
        super().__init__(type_name, type_namespace)
        self.is_simple_not_complex = True

    def __str__(self):
        return f"simple; {self.type_name}; {self.type_namespace}"

    @abc.abstractmethod
    def get_base_type_functions(self):
        raise NotImplementedError

    def get_simple_types(self):
        return {str(self.__class__.__name__): self}

    def get_child_types(self):
        return [self]


class BoolType(SimpleType):

    def __str__(self):
        return f"BoolType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        return [self.encode_function, self.decode_function]

    @property
    def encode_function(self):
        return CppFunction(function_name="injectBoolValue",
                           return_type="void", arguments="bool",
                           comment="/ \\brief writes the given boolean value to the bitstream",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name="extractBoolValue",
                           return_type="bool", arguments=None,
                           comment="/ \\brief read a bit from bitstream and returns the interpreted value",
                           code="", member_object_name="base_types_")

    def generate_code(self):
        return ""


class Base64Type(SimpleType):

    def __str__(self):
        return f"Base64Type; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        return [self.encode_function, self.decode_function]

    @property
    def encode_function(self):
        return CppFunction(function_name="injectBase64Value",
                           return_type="void", arguments="std::string value",
                           comment="",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name="extractBase64Value",
                           return_type="std::string", arguments=None,
                           comment="",
                           code="", member_object_name="base_types_")


class EnumType(SimpleType):

    def __init__(self, type_name: str, type_namespace: str, enumerations: List[str]):
        super().__init__(type_name, type_namespace)
        self.enumerations = enumerations

    def __str__(self):
        return f"EnumType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractNBitsForEnum",
                                     return_type="uint32_t", arguments="uint32_t n_bits",
                                     comment="",
                                     code="", member_object_name="base_types_"))
        return functions

    @property
    def encode_function(self):
        return CppFunction(function_name=f"encode_{self.type_name}",
                           return_type="void", arguments="std::string value",
                           comment="",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name=f"decode_{self.type_name}",
                           return_type="std::string", arguments=None,
                           comment="",
                           code="", member_object_name="enum_types_")


class StringType(SimpleType):

    def __str__(self):
        return f"StringType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        return [self.encode_function, self.decode_function]

    @property
    def encode_function(self):
        return CppFunction(function_name="injectString",
                           return_type="void", arguments="std::string value",
                           comment="",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name="extractString",
                           return_type="std::string", arguments=None,
                           comment="",
                           code="", member_object_name="base_types_")


class DecimalType(SimpleType):
    TYPE_SIZE = {'long': 8, 'int': 4, 'integer': 4, 'short': 2, 'byte': 1, 'HMACOutputLengthType': 4}

    def __init__(self, type_name: str, type_namespace: str, detail_type, min_val, max_val):
        super().__init__(type_name, type_namespace)
        self.detail_type = detail_type
        self.min_val = min_val
        self.max_val = max_val
        try:
            self.num_bytes = self.TYPE_SIZE[self.detail_type]
        except KeyError:
            self.num_bytes = ceil(log2(max_val)/8)
        self.is_unsigned = min_val == 0

    def __str__(self):
        return f"DecimalType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        return [self.encode_function, self.decode_function]

    @property
    def encode_function(self):
        return CppFunction(function_name="injectNaturalNumber",
                           return_type="void", arguments="int32_t number",
                           comment="",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name="extractIntegerNumber",
                           return_type="int32_t", arguments="uint8_t n_bytes, bool is_unsigned",
                           comment="",
                           code="", member_object_name="base_types_", call_args=f"{self.num_bytes}, {str(self.is_unsigned).lower()}")


class HexBinType(SimpleType):

    def __init__(self, type_name: str, type_namespace: str, max_length: int):
        super().__init__(type_name, type_namespace)
        self.max_length = max_length

    def __str__(self):
        return f"HexBinType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        return [self.encode_function, self.decode_function]

    @property
    def encode_function(self):
        return CppFunction(function_name="injectHexBin",
                           return_type="void", arguments="std::string",
                           comment="",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name="extractHexBin",
                           return_type="std::string", arguments="uint8_t max_length",
                           comment="",
                           code="", member_object_name="base_types_", call_args=f"{self.max_length}")
