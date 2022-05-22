import abc


class BaseType(metaclass=abc.ABCMeta):

    def __init__(self, type_name: str, type_namespace: str):
        self.is_simple_not_complex = None
        self.type_name = type_name
        self.type_namespace = type_namespace

    @property
    @abc.abstractmethod
    def encode_function(self):
        raise NotImplementedError

    @property
    @abc.abstractmethod
    def decode_function(self):
        raise NotImplementedError

    @abc.abstractmethod
    def get_simple_types(self):
        raise NotImplementedError

    @abc.abstractmethod
    def get_child_types(self):
        raise NotImplementedError
