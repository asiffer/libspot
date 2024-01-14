import ctypes
import os
import sys

from setuptools import Extension, setup  # type: ignore
from wheel.bdist_wheel import bdist_wheel  # type: ignore

ROOT = "../"


INCLUDE_DIRS = [os.path.join(ROOT, "include"), os.path.join(ROOT, "dist")]
SRC_DIR = os.path.join(ROOT, "src")
SOURCES = [
    os.path.join(ROOT, "src", f)
    for f in os.listdir(os.path.join(ROOT, "src"))
    if f.endswith(".c")
] + ["libspotmodule.c"]


def get_version() -> str:
    """Return libspot version from the Makefile"""
    makefile = open(
        os.path.join(ROOT, "Makefile"),
        "r",
        encoding="utf-8",
    ).read()
    i = makefile.find("VERSION")
    j = makefile.find("\n", i)
    return makefile[i:j].replace("VERSION", "").replace("=", "").strip()


class bdist_wheel_abi3(bdist_wheel):
    def get_tag(self):
        python, abi, plat = super().get_tag()

        if python.startswith("cp"):
            # on CPython, our wheels are abi3 and compatible back to 3.6
            return "cp36", "abi3", plat

        return python, abi, plat


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
    include_dirs=INCLUDE_DIRS,
    sources=SOURCES,
    extra_compile_args=["-std=c99"],
    define_macros=define_macros,
    py_limited_api=True,
)

# other parameters are defined in pyproject.toml
setup(
    version=get_version(),
    test_suite="test.Test",
    ext_modules=[lib],
    cmdclass={"bdist_wheel": bdist_wheel_abi3},
)
