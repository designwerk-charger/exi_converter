import exi_converter  # pylint: disable=import-error

from common.utils.logger import get_logger
from iso15118.shared.iexi_codec import IEXICodec


class CppExiCodec(IEXICodec):

    def __init__(self):
        self.logger = get_logger(self)
        self.codec = exi_converter.ExiCodec()

    def encode(self, message: str, namespace: str) -> bytes:
        return self.codec.encode(message, namespace)

    def decode(self, stream: bytes, namespace: str) -> str:
        return self.codec.decode(stream, len(stream), namespace)

    def get_version(self) -> str:
        return exi_converter.__version__
