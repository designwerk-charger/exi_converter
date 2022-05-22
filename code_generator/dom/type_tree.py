from typing import List

import xmlschema
from xmlschema import XsdElement, XsdAttribute, XsdType, XsdComponent

from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element
from datatypes.simple_type import SimpleType, EnumType, StringType, DecimalType, BoolType, HexBinType, Base64Type
from dom.ccs_messages import ISO15118_2_MSGS


class TypeTree:

    def __init__(self, schema_file:str):
        self._schema = xmlschema.XMLSchema(source=schema_file)

        self._type_tree = []
        top_level_msgs = self.get_full_list_of_message_types(ISO15118_2_MSGS)
        for name, types in self._schema.types.target_dict.items():
            if types.local_name in top_level_msgs:
                self._type_tree.append(self.diveIntoType(types))

    @property
    def type_tree(self) -> List[BaseType]:
        return self._type_tree

    @staticmethod
    def get_full_list_of_message_types(msgs: List):
        full_list = []
        for msg in msgs:
            full_list.append(f"{msg}ResType")
            full_list.append(f"{msg}ReqType")
        return full_list

    @staticmethod
    def extractComponentType(c: XsdComponent):
        if c.type.name in c.schema.types.target_dict.keys():
            return c.schema.types.target_dict[c.type.name]
        else:
            raise KeyError(f"The type ({c.type.name}) for {c.name} does not exist.")

    @staticmethod
    def getSimpleTypeInfo(t: XsdType) -> SimpleType:
        if not t.is_simple():
            raise TypeError(f"The type {t.local_name} is not simple as expected!")

        if t.sequence_type == "xs:string":
            if t.enumeration:
                return EnumType(t.prefixed_name, "enum", t.enumeration)
            else:
                return StringType("string", t.sequence_type) #{t.name}
        elif t.sequence_type == "xs:decimal":
            return DecimalType("decimal", t.sequence_type, detail_type=t.local_name, min_val=t.min_value, max_val=t.max_value)
        elif t.sequence_type == "xs:float":
            raise TypeError(f"Float type ist not supported! {t}")
        elif t.sequence_type == "xs:boolean":
            return BoolType("bool", t.sequence_type)
        elif t.sequence_type == "xs:hexBinary":
            return HexBinType("hexBin", t.sequence_type, max_length=t.max_length)
        elif t.sequence_type == "xs:base64Binary":
            return Base64Type("base64", t.sequence_type)
        else:
            raise TypeError(f"The type {t.sequence_type} ist not supported! {t}")

    @staticmethod
    def diveIntoType(t: XsdType) -> BaseType:
        if t.is_simple():
            return TypeTree.getSimpleTypeInfo(t)
        else:
            components = []
            for c in t.iter_components():
                if isinstance(c, XsdElement):
                    is_optional = True if c.min_occurs == 0 else False
                    components.append(Element(c.local_name, TypeTree.diveIntoType(TypeTree.extractComponentType(c)),
                                              is_optional=is_optional, max_items=c.max_occurs))
                if isinstance(c, XsdAttribute):
                    components.append(Element(c.local_name, TypeTree.diveIntoType(TypeTree.extractComponentType(c)), False))
            return ComplexType(t.local_name, t.sequence_type, components)
