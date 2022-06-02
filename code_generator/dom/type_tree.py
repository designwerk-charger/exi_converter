import collections
from typing import List

import xmlschema
from xmlschema import XsdElement, XsdAttribute, XsdType, XsdComponent


from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element, Attribute
from datatypes.simple_type import SimpleType, EnumType, StringType, DecimalType, BoolType, HexBinType, Base64Type, \
    IgnoredType
from dom.ccs_messages import ISO15118_2_MSGS, ISO15118_2_MAIN_TYPES


class TypeTree:
    IGNORED_TYPES = ["xs:anyURI", "xs:fullDerivationSet", "xs:simpleDerivationSet", 'xs:derivationSet', 'xs:allNNI', 'xs:blockSet',
                     'xs:namespaceList', 'xs:float', 'xs:double', 'xs:duration', 'xs:time', 'xs:date', 'xs:dateTime',
                     'xs:gYearMonth', 'xs:gYear', 'xs:gMonth', 'xs:gDay', 'xs:gMonthDay', 'xs:QName', 'xs:NOTATION',
                     'xs:string+', 'xs:anySimpleType']

    def __init__(self, schema_file:str):
        self._schema = xmlschema.XMLSchema(source=schema_file)

        self._all_types = TypeTree.extract_all_types(self._schema.types.target_dict.values())
        TypeTree.update_derivations(self._all_types)

        self._type_tree = []
        top_level_msgs = self.get_full_list_of_message_types(ISO15118_2_MSGS) + ISO15118_2_MAIN_TYPES
        top_level_msgs.append("V2G_Message")
        for name, types in self._schema.types.target_dict.items():
            if types.local_name in top_level_msgs:
                self._type_tree.append(self.diveIntoType(types, self._all_types))

        print(f"Type Tree created with {len(self._type_tree)} top level types")

    @staticmethod
    def extract_all_types(schema_types: List[XsdType]) -> List[BaseType]:
        all_types = []
        for t in schema_types:
            if t.is_simple():
                all_types.append(TypeTree.getSimpleTypeInfo(t))
            else:
                base_type = None if t.base_type == None else t.base_type.local_name
                all_types.append(ComplexType(t.local_name, t.qualified_name, base_type, t.abstract))
        return all_types

    @staticmethod
    def update_derivations(all_types: List[BaseType]):
        for abstract_type in all_types:
            if abstract_type.is_abstract:
                if not isinstance(abstract_type, ComplexType):
                    raise RuntimeError(f"The type {abstract_type} is abstract but not complex!")
                for t in all_types:
                    if abstract_type.type_name == t.base_class:
                        abstract_type.derived_classes.append(t)


    @property
    def schema(self):
        return self._schema

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
        if c.type.qualified_name in c.schema.types.target_dict.keys():
            return c.schema.types.target_dict[c.type.qualified_name]
        else:
            raise KeyError(f"The type ({c.type.qualified_name}) for {c.name} does not exist.")

    @staticmethod
    def getSimpleTypeInfo(t: XsdType) -> SimpleType:
        if not t.is_simple():
            raise TypeError(f"The type {t.qualified_name} is not simple as expected!")

        if t.sequence_type == "xs:string":
            if t.enumeration:
                return EnumType(t.prefixed_name, type_namespace=t.qualified_name, enumerations=t.enumeration)
            else:
                return StringType("string", type_namespace=t.qualified_name)
        elif t.sequence_type == "xs:decimal":
            return DecimalType("decimal", type_namespace=t.qualified_name, detail_type=t.local_name, min_val=t.min_value, max_val=t.max_value)
        elif t.sequence_type == "xs:boolean":
            return BoolType("bool", type_namespace=t.qualified_name)
        elif t.sequence_type == "xs:hexBinary":
            return HexBinType("hexBin", type_namespace=t.qualified_name, max_length=t.max_length)
        elif t.sequence_type == "xs:base64Binary":
            return Base64Type("base64", type_namespace=t.qualified_name)
        elif t.sequence_type in TypeTree.IGNORED_TYPES:
            return IgnoredType(type_namespace=t.qualified_name)
        else:
            raise TypeError(f"The type {t.qualified_name} ist not supported! {t}")

    @staticmethod
    def get_substitutes(element: XsdElement, all_types: List[BaseType]):
        stt = {}
        for s in element.iter_substitutes():
            stt[s.local_name] = TypeTree.diveIntoType(s.type, all_types)
        return collections.OrderedDict(sorted(stt.items()))

    @staticmethod
    def diveIntoType(t: XsdType, all_types: List[BaseType]) -> BaseType:
        if t.qualified_name not in all_types:
            raise RuntimeError(f"The type {t.qualified_name} is not available in available types")

        refered_item = all_types[all_types.index(t.qualified_name)]

        if t.is_simple():
            return refered_item
        elif isinstance(refered_item, ComplexType):
            components = []
            for c in t.iter_components():
                if isinstance(c, XsdElement):
                    is_optional = True if c.min_occurs == 0 else False
                    xsd_type = TypeTree.extractComponentType(c)
                    substitutes = TypeTree.get_substitutes(c, all_types)
                    components.append(Element(c.local_name,
                                              TypeTree.diveIntoType(xsd_type, all_types),
                                              is_optional=is_optional, max_items=c.max_occurs, substitutes=substitutes))
                if isinstance(c, XsdAttribute):
                    optional = True if c.use == "optional" else False
                    attr_type = TypeTree.diveIntoType(TypeTree.extractComponentType(c), all_types)
                    if not isinstance(attr_type, SimpleType):
                        raise TypeError(f"Attributes can only be simple types! got type '{attr_type.type_name}' for "
                                        f"component '{c.local_name}' in '{t.local_name}'")
                    if len(components) > 0 and not isinstance(components[-1], Attribute):
                        raise RuntimeError(f"Attributes are not allowed after Elements! "
                                           f"component '{c.local_name}' in '{t.local_name}'")
                    if len(components) > 0 and components[-1].element_name > c.local_name:
                        raise RuntimeError(f"Attributes are not lexicographical ordered! "
                                           f"new component '{c.local_name}' vs. "
                                           f"existing component '{components[-1].element_name}' in '{t.local_name}'")
                    components.append(Attribute(c.local_name, attr_type, is_optional=optional))
            refered_item.child_elements = components
            return refered_item
        raise RuntimeError(f"the Type {t.local_name} is neither simple nor complex!")