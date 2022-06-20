from math import ceil, log2
from typing import List

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element, Attribute, AnyElement


class ComplexTypes:

    def __init__(self, type_tree: List[BaseType]):
        self._local_suffix_cnt = 0
        self.all_types = {}
        for item in type_tree:
            for type in item.get_child_types():
                self.all_types[type.base_type_name] = type

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
            ct_func = self.getDecodeFunction(ct)
            self.cpp_class.add_function(ct_func)

    @property
    def local_suffix_cnt(self):
        self._local_suffix_cnt += 1
        return self._local_suffix_cnt

    @staticmethod
    def _decode_simple_element(element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        optional_str = "optional " if element.is_optional else ""
        return_str = f"{indent_str}{{  // decode {optional_str}simple {element.__class__.__name__} type\n"
        if isinstance(element, Element) and not from_optional:
            return_str += f"{indent_str}\tbase_types_->check_event_code_is_0(\"Start+{element.element_name}\");\n"
        return return_str + \
               f"{indent_str}\tstring_stream_->start_key(\"{element.element_name}\");\n" \
               f"{indent_str}\tauto var = {element.element_type.decode_function.call()};\n" \
               f"{indent_str}\tstring_stream_->add_value(var);\n" \
               f"{indent_str}\t#ifndef NDEBUG\n" \
               f"{indent_str}\t\tstd::cout << \"getting value for {optional_str}{element.__class__.__name__} '{element.element_name}' -> \" << var << std::endl;\n" \
               f"{indent_str}\t#endif\n" \
               f"{indent_str}\tstring_stream_->end_key();\n" \
               f"{indent_str}}}\n"

    @staticmethod
    def _decode_simple_element_with_event_code(element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        if isinstance(element, Attribute):
            return f"{ComplexTypes._decode_simple_element(element, indent, from_optional)}"
        return f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
               f"{ComplexTypes._decode_simple_element(element, indent, from_optional)}" \
               f"{indent_str}base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"

    @staticmethod
    def _decode_complex_element(element_name: str, element_type: ComplexType, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}string_stream_->start_key(\"{element_name}\");\n" \
               f"{indent_str}{element_type.decode_function.call()};\n" \
               f"{indent_str}string_stream_->end_key();\n"

    @staticmethod
    def _do_abstract_element_checks(element: Element):
        if not isinstance(element.element_type, ComplexType):
            raise NotImplementedError(f"The type of Element {element.element_name} is not ComplexType")
        if len(element.element_type.derived_classes) == 0:
            print(f"WARNING: abstract class {element.element_type.type_name} has no derived classes")
        if len(element.element_type.derived_classes) != len(element.substitutes):
            print(f"WARNING: abstract class {element.element_type.type_name} has not same number of derivations as the element has substitutes")
        if element.is_optional:
            print(f"WARNING: element {element.element_name} with abstract class {element.element_type.type_name} is optional!")

    def _decode_complex_element_with_event_code(self, element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent

        return_str = f"{indent_str}// decode complex {element.__class__.__name__} type\n"
        if not from_optional:
            return_str += f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n"
        return_str += f"{ComplexTypes._decode_complex_element(element.element_name, element.element_type, indent)}"
        if len(element.element_type.child_elements) == 0 or not element.element_type.child_elements[-1].is_list:
            if not from_optional:
                return_str += f"{indent_str}// base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"  # Todo: check if the end element is needed
            else:
                return_str += f"{indent_str}// base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"  # Todo: check if the end element is needed
        else:
            return_str += f"{indent_str}// Skipping check_event_code_is_0 for End{element.element_name} because of list\n"
        return return_str

    def decode_element_with_event_code(self, element: Element, indent: int, from_optional=False) -> str:
        if element.element_type.is_simple_not_complex:
            return ComplexTypes._decode_simple_element_with_event_code(element, indent, from_optional)
        return self._decode_complex_element_with_event_code(element, indent, from_optional)

    def decodeElementAsList(self, element: Element, indent: int) -> str:
        suffix = self.local_suffix_cnt
        code = f"base_types_->check_event_code_is_0(\"StartList{element.element_name}\");\n" \
               f"string_stream_->start_key(\"{element.element_name}\");\n" \
               f"string_stream_->start_list();\n"

        code += f"int further_items{suffix};\n" \
                f"for(int i=0; i<{element.max_items}; i++) {{;\n" \
                f"\tif (i!=0) {{\n" \
                f"\t\tfurther_items{suffix} = base_types_->get_event_code_with_n_bits(2, \"ListItem{element.element_name}\");\n" \
                f"\t\tif (further_items{suffix} == 1) {{\n" \
                f"\t\t\tbreak;\n" \
                f"\t\t}} else if (further_items{suffix} == 0) {{\n" \
                f"\t\t\tstring_stream_->next_item();\n" \
                f"\t\t}} else {{\n" \
                f"\t\t\tthrow std::runtime_error(\"Unexpected code while decoding {element.element_name} List!\");\n" \
                f"\t\t}}\n" \
                f"\t}}\n"

        if element.element_type.is_simple_not_complex:
            code += f"\t{{\n" \
                    f"\t\tbase_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
                    f"\t\tauto var = {element.element_type.decode_function.call()};\n" \
                    f"\t\tstring_stream_->add_value(var);\n" \
                    f"\t\t#ifndef NDEBUG\n" \
                    f"\t\t\tstd::cout << \"getting value for {element.__class__.__name__} '{element.element_name}' -> \" << var << std::endl;\n" \
                    f"\t\t#endif\n" \
                    f"\t\tbase_types_->check_event_code_is_0(\"End{element.element_name}\");\n" \
                    f"\t}}\n"
        else:
            code += f"{element.element_type.decode_function.call()};\n"

        code += f"}}\n" \
                f"string_stream_->end_list();\n" \
                f"string_stream_->end_key();\n" \
                f"// base_types_->check_event_code_is_0(\"EndList{element.element_name}\");\n"  # Todo: check if the end element is needed

        return code

    def sortOptionalBlobElements(self, elements: List[Element]) -> str:
        anyE = []
        E = []
        A = []
        for e in elements:
            if isinstance(e, AnyElement):
                anyE.append(e)
            elif isinstance(e, Attribute):
                A.append(e)
            elif isinstance(e, Element):
                E.append(e)
            else:
                raise RuntimeError("Unexpected type")
        return A + E + anyE

    def getDecodeCodeForOptionalBlob(self, elements: List[Element], is_last: bool) -> str:
        def get_num_AnyElements() -> int:
            cnt = 0
            for e in elements:
                if isinstance(e, AnyElement):
                    cnt += 1
            return cnt

        def needs_endelement() -> bool:
            has_min_1_optional = any([e.is_optional for e in elements])
            if elements[-1].element_type.is_abstract and not has_min_1_optional:
                # if last Elements is abstract. there is no need for end element
                return False
            if is_last:
                return True
            if elements[-1].is_optional: # or elements[-1].element_type.is_abstract:
                return True
            return False

        def get_num_eventcode_bits(progress=0) -> int:
            cnt = len(elements) - progress + 1 + get_num_AnyElements()  # +1 because one optional parameter gets at least 2 bits reading

            if needs_endelement():
                cnt += 1
            return ceil(log2(cnt))

        suffix = self.local_suffix_cnt
        names = "_".join([e.element_name for e in elements])
        end_str = " + END_ELEMENT" if needs_endelement() else ""
        code = f"// Decoding optional elements: {[en.element_name for en in elements]}{end_str}\n" \
               f"uint8_t ec{suffix} = base_types_->get_event_code_with_n_bits({max(get_num_eventcode_bits(), 2)}, \"Start{names}\");\n"
        code += f"bool continue_loop{suffix} = true;\n" \
                f"while(continue_loop{suffix}){{\n"
        code += f"switch(ec{suffix}) {{\n"
        for i, element in enumerate(elements):
            code += f"\tcase {i}:\n"
            if element.is_list:
                code += self.decodeElementAsList(element, 2)
            else:
                code += f"{self.decode_element_with_event_code(element, 2, from_optional=True)}"
            if element.is_optional:
                code += f"\t\tec{suffix} = {i + 1} + base_types_->get_event_code_with_n_bits(" \
                        f"{max(1, get_num_eventcode_bits(i+1))}, \"Start{names}{i}\");\n"
            else:
                code += f"\t\t continue_loop{suffix} = false;\n"
            code += f"\t\tbreak;\n"

        if elements[-1].is_optional:  # special case for optional messages at the end of a complex type
            code += f"\tcase {len(elements)}:\n" \
                    f"\t\tcontinue_loop{suffix} = false;\n" \
                    f"\t\tbreak;\n"
        code += f"\tdefault:\n" \
                f"\t\tthrow std::runtime_error(\"While parsing event code for '{names}' a unexpected code (\" +  std::to_string(ec{suffix}) + \") appeared  0!\");\n" \
                f"}}}}"
        return code

    def getDecodeFunction(self, ct: ComplexType):
        code = ""
        optional_blob = []
        was_normal_complex = True
        for element in ct.child_elements:
            if element.is_list:
                if element.is_optional or len(optional_blob) != 0:
                    print(f"WARNING: list as optional types not yet tested ({element.element_name})")
                    optional_blob.append(element)
                else:
                    code += self.decodeElementAsList(element, 0)
                    was_normal_complex = False
            elif element.element_type.is_abstract:
                self._do_abstract_element_checks(element)
                for (element_name, element_type) in element.substitutes.items():
                    optional_blob.append(Element(element_name, element_type, is_optional=False, max_items=1, substitutes={}))
            elif element.is_optional:
                optional_blob.append(element)
            elif len(optional_blob) != 0:
                optional_blob.append(element)
                sorted_blob = self.sortOptionalBlobElements(optional_blob)
                code += self.getDecodeCodeForOptionalBlob(sorted_blob, is_last=False)
                code += "\n"
                optional_blob = []
                was_normal_complex = not sorted_blob[-1].is_optional
            else:
                code += self.decode_element_with_event_code(element, 0)
                code += "\n"
                was_normal_complex = True
        if len(optional_blob) != 0:
            sorted_blob = self.sortOptionalBlobElements(optional_blob)
            code += self.getDecodeCodeForOptionalBlob(sorted_blob, is_last=True)
            was_normal_complex = not sorted_blob[-1].is_optional

        if was_normal_complex:
            code += f"base_types_->check_event_code_is_0(\"Finish{ct.type_name}\");\n"

        return CppFunction(function_name=ct.decode_function.function_name,
                           return_type="void",
                           arguments=None,
                           code=code,
                           comment=None)

    def write_complex_type_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_complex_type_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
