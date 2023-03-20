HEADER = """
import os
from ctypes import (
    CDLL,
    CFUNCTYPE,
    POINTER,
    Structure,
    c_char_p,
    c_double,
    c_int,
    c_ulong,
    c_void_p,
)
from ctypes.util import find_library


def find_libspot() -> CDLL:
    here = os.path.dirname(__file__)
    for file in os.listdir(here):
        if file.startswith("libspot"):
            return CDLL(os.path.join(here, file))
    lib = find_library("spot")
    if lib:
        return CDLL(lib)
    raise RuntimeError("libspot not found")

{lib} = find_libspot()
"""

STRUCT = """
class {name}(Structure):
    _fields_ = [
{fields}
    ]"""

FUNCTION = """
{lib}.{name}.argtypes = [{args}]
{lib}.{name}.restype = {type}"""

FUNCTION_TYPEDEF = """
{name} = CFUNCTYPE({type}, {args})"""
