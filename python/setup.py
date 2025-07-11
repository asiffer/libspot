import ctypes
import os
import sys
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.bdist_wheel import bdist_wheel

ROOT = Path("../")


INCLUDE_DIRS = [ROOT / "include", ROOT / "dist"]
SRC_DIR = ROOT / "src"
SOURCES = [SRC_DIR / f for f in os.listdir(SRC_DIR) if f.endswith(".c")] + [
    "libspotmodule.c"
]

C99_ARG = r"/std:c99" if sys.platform == "win32" else r"-std=c99"


class bdist_wheel_abi3(bdist_wheel):
    def get_tag(self):
        python, abi, plat = super().get_tag()

        if python.startswith("cp"):
            # on CPython, our wheels are abi3 and compatible back to 3.6
            return "cp36", "abi3", plat

        return python, abi, plat


print(ROOT, SRC_DIR, INCLUDE_DIRS, SOURCES)


def get_version() -> str:
    """Return libspot version from the Makefile"""
    makefile = open(
        ROOT / "Makefile",
        "r",
        encoding="utf-8",
    ).read()
    i = makefile.find("VERSION")
    j = makefile.find("\n", i)
    return makefile[i:j].replace("VERSION", "").replace("=", "").strip()


define_macros = [
    ("VERSION", f'"{get_version()}"'),
    ("Py_LIMITED_API", "0x03060000"),  # macro to use Python Limited API (here >=cp36)
]

# windows specific
if sys.platform == "win32":
    # define macros manually based on ctypes output
    sizeof_double = ctypes.sizeof(ctypes.c_double)
    size_max = hex(pow(2, 8 * ctypes.sizeof(ctypes.c_size_t)) - 1)
    size = {
        "unsigned": ctypes.sizeof(ctypes.c_uint),
        "unsigned long": ctypes.sizeof(ctypes.c_ulong),
        "unsigned long long": ctypes.sizeof(ctypes.c_ulonglong),
    }
    uint32_type = next(key for key, value in size.items() if value == 4)
    uint64_type = next(key for key, value in size.items() if value == 8)

    define_macros += [
        ("__SIZEOF_DOUBLE__", sizeof_double),
        ("__UINT32_TYPE__", uint32_type),
        ("__UINT64_TYPE__", uint64_type),
        ("__SIZE_MAX__", size_max),
    ]


lib = Extension(
    "libspot",
    language="c",
    include_dirs=list(map(str, INCLUDE_DIRS)),
    sources=list(map(str, SOURCES)),
    extra_compile_args=[C99_ARG],
    define_macros=define_macros,  # type: ignore
    py_limited_api=True,
)

# other parameters are defined in pyproject.toml
setup(
    version=get_version(),
    test_suite="test.Test",
    ext_modules=[lib],
    cmdclass={"bdist_wheel": bdist_wheel_abi3},
)
