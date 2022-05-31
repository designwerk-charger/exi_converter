from math import ceil, log2
from typing import List

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element
from datatypes.simple_type import EnumType


class ComplexTypes:

    def __init__(self, type_tree: List[BaseType]):
        self.optional_parameter_suffix_cnt = 0
        self.all_types = {}
        for item in type_tree:
            if item.type_name == "DC_EVChargeParameterType":
                print("adfsadf")
            for type in item.get_child_types():
                self.all_types[type.type_name] = type

        self.all_complex_types = []
        for t in self.all_types.values():
            if isinstance(t, ComplexType):
                self.all_complex_types.append(t)

        self.cpp_class = CppClass(class_name="ComplexTypes", derived_from_class=None,
                                  includes="#include <iostream>\n#include <cstdint>\n#include <cstdio>\n#include <string>\n#include <sstream>\n#include <unordered_map>\n"
                                           "#include \"base_types.h\"\n#include \"enum_types.h\"\n#include \"base/stringstream.h\"\n")
        self.cpp_class.add_member("BaseTypes * base_types_;\n\tEnumTypes * enum_types_;\n\tStringStream * string_stream_;")
        self.cpp_class.add_constructor("BaseTypes * base_types, EnumTypes * enum_types, StringStream * string_stream",
                                       "base_types_ = base_types;\nenum_types_ = enum_types;\nstring_stream_ = string_stream;\n")

        for ct in self.all_complex_types:
            ct_with_base = self.addBaseElementsToCt(ct)
            ct_func = self.getDecodeFunction(ct_with_base)
            self.cpp_class.add_function(ct_func)

    def addBaseElementsToCt(self, ct: ComplexType):
        if ct.base_class:
            for t in self.all_types.values():
                if t.type_name == ct.base_class:
                    if isinstance(t, ComplexType):
                        new_elements = t.child_elements + ct.child_elements
                        ct.child_elements = new_elements
                    return ct
            print(f"Error: no baseclass found for {ct}")
            #raise RuntimeError(f"no baseclass found for {ct}")
        return ct

    @staticmethod
    def _decode_simple_element(element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        return_str = ""
        #if isinstance(element.element_type, EnumType):
        #    return_str += f"{indent_str}base_types_->check_event_code_is_0(\"StartEnumeration\");\n"
        if element.element_name in ["NotificationMaxDelay", "EVSENotification"]:
            return_str += f"{indent_str}base_types_->check_event_code_is_0(\"Start+{element.element_name}\");\n"
        return return_str + \
               f"{indent_str}string_stream_->start_key(\"{element.element_name}\");\n" \
               f"{indent_str}#ifndef NDEBUG\n" \
               f"{indent_str}\tstd::cout << \"getting value for '{element.element_name}'\" << std::endl;\n" \
               f"{indent_str}#endif\n" \
               f"{indent_str}string_stream_->add_value({element.element_type.decode_function.call()});\n" \
               f"{indent_str}string_stream_->end_key();\n"

    @staticmethod
    def _decode_simple_element_with_event_code(element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}// decode simple type\n" \
               f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
               f"{ComplexTypes._decode_simple_element(element, indent)}" \
               f"{indent_str}base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"

    @staticmethod
    def _decode_complex_element(element_name: str, element_type: ComplexType, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}string_stream_->start_key(\"{element_name}\");\n" \
               f"{indent_str}{element_type.decode_function.call()};\n" \
               f"{indent_str}string_stream_->end_key();\n"


    def _decode_complex_element_with_event_code(self, element: Element, indent: int) -> str:
        def get_num_abstract_classes_bits() -> int:
            return ceil(log2(len(element_type.derived_classes)+1)) # +1 because one optional parameter gets at least 2 bits reading

        indent_str = "\t" * indent

        if element.element_type.is_abstract:
            element_type = element.element_type
            if not isinstance(element_type, ComplexType):
                raise NotImplementedError(f"The type of Element {element.element_name} is not ComplexType")
            if len(element_type.derived_classes) == 0:
                print(f"WARNING: class {element_type.type_name} has no derived classes")
            code = f"{indent_str}int classtype{self.optional_parameter_suffix_cnt} = base_types_->get_event_code_with_n_bits({get_num_abstract_classes_bits()}, \"Start{element.element_name}\");\n" \
                   f"{indent_str}switch(classtype{self.optional_parameter_suffix_cnt}) {{\n"
            for i, type in enumerate(element_type.derived_classes):
                   code += f"{indent_str}\tcase {i}:\n" \
                           f"{self._decode_complex_element(element.element_name, type, indent+2)}" \
                           f"{indent_str}\t\tbreak;\n"
            code += f"}}\n"
            return code

        return f"{indent_str}// decode complex type\n" \
               f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
               f"{ComplexTypes._decode_complex_element(element.element_name, element.element_type, indent)}" \
               f"{indent_str}base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"

    @staticmethod
    def decode_element(element: Element, indent: int) -> str:
        if element.element_type.is_simple_not_complex:
            return ComplexTypes._decode_simple_element(element, indent)
        return ComplexTypes._decode_complex_element(element.element_name, element.element_type, indent)

    def decode_element_with_event_code(self, element: Element, indent: int) -> str:
        if element.element_type.is_simple_not_complex:
            return ComplexTypes._decode_simple_element_with_event_code(element, indent)
        return self._decode_complex_element_with_event_code(element, indent)


    def decodeElementAsList(self, element: Element, indent: int) -> str:
        code = f"base_types_->check_event_code_is_0(\"StartList{element.element_name}\");\n"

        code += f"int further_items{self.optional_parameter_suffix_cnt};\n" \
                f"for(int i=0; i<{element.max_items}; i++) {{;\n" \
                f"\tif (i!=0) {{\n" \
                f"\t\tfurther_items{self.optional_parameter_suffix_cnt} = base_types_->get_event_code_with_n_bits(2, \"ListItem{element.element_name}\");\n"
        code += f"\t\tif (further_items{self.optional_parameter_suffix_cnt} == 1) {{\n" \
                f"\t\t\tbreak;\n" \
                f"\t\t}} else if (further_items{self.optional_parameter_suffix_cnt} == 0) {{\n"
        code += f"\t\t}} else {{\n" \
                f"\t\t\tthrow std::runtime_error(\"Unexpected code while decoding {element.element_name} List!\");\n" \
                f"\t\t}}\n" \
                f"\t}}\n"

        if element.element_type.is_simple_not_complex:
            code += ComplexTypes._decode_simple_element(element, indent+1)
        else:
            code += ComplexTypes._decode_complex_element(element.element_name, element.element_type, indent+1)

        code += f"}}\n" \
                f"base_types_->check_event_code_is_0(\"EndList{element.element_name}\");\n"

        return code


    def getDecodeCodeForOptionalBlob(self, elements: List[Element], indent: int) -> str:
        def get_num_eventcode_bits(progress=0) -> int:
            return ceil(log2(len(elements)+1-progress)) # +1 because one optional parameter gets at least 2 bits reading
        self.optional_parameter_suffix_cnt += 1
        indent_str = "\t" * indent
        names = "_".join([e.element_name for e in elements])
        code = f"{indent_str}uint8_t ec{self.optional_parameter_suffix_cnt} = base_types_->get_event_code_with_n_bits({get_num_eventcode_bits()}, \"Start{names}\");\n"
        code += f"{indent_str}bool continue_loop{self.optional_parameter_suffix_cnt} = true;\n" \
                f"{indent_str}while(continue_loop{self.optional_parameter_suffix_cnt}){{\n"
        code += f"{indent_str}switch(ec{self.optional_parameter_suffix_cnt}) {{\n"
        for i, element in enumerate(elements):
            code += f"{indent_str}\tcase {i}:\n"
            code += f"{indent_str}{self.decode_element_with_event_code(element, indent+2)}"
            if element.is_optional:
                code += f"{indent_str}\t\tec{self.optional_parameter_suffix_cnt} = {i+1} + base_types_->get_event_code_with_n_bits({get_num_eventcode_bits(i+1)}, \"Start{names}{i}\");\n"
            else:
                code += f"{indent_str}\t\t continue_loop{self.optional_parameter_suffix_cnt} = false;\n"
            code += f"{indent_str}\t\tbreak;\n"
        if elements[-1].is_optional:  # special case for optional messages at the end of a complex type
            code += f"{indent_str}\tcase {len(elements)}: continue_loop{self.optional_parameter_suffix_cnt} = false; break;\n"
        code += f"{indent_str}\tdefault:\n" \
                f"{indent_str}\t\tthrow std::runtime_error(\"While parsing event code for '{names}' a unexpected code (\" +  std::to_string(ec{self.optional_parameter_suffix_cnt}) + \") appeared  0!\");\n" \
                f"{indent_str}}}}}"
        return code

    def getDecodeFunction(self, ct: ComplexType):
        code = ""
        optional_blob = []
        for element in ct.child_elements:
            if element.element_name in ["Reference", "Transform", "Object"]:
                print(f"WARNING: object not handled ({element})!")
            elif element.is_list:
                if element.is_optional or len(optional_blob) != 0:
                    raise NotImplementedError(f"list is not yet implemented for optional types or preciding optional types! ({element})")
                code += self.decodeElementAsList(element, 0)
            elif element.is_optional:
                optional_blob.append(element)
            elif len(optional_blob) != 0:
                optional_blob.append(element)
                code += self.getDecodeCodeForOptionalBlob(optional_blob, 0)
                code += "\n"
                optional_blob = []
            else:
                code += self.decode_element_with_event_code(element, 0)
                code += "\n"
        if len(optional_blob) != 0:
            code += self.getDecodeCodeForOptionalBlob(optional_blob, 0)

        return CppFunction(function_name=ct.decode_function.function_name,
                           return_type="void",
                           arguments="void",
                           code=code,
                           comment=None)

    def write_complex_type_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_complex_type_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
