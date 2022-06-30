#!/usr/bin/python3
# -*- coding: utf-8 -*-

# from setuptools.command.build_ext import build_ext
# from setuptools.extension import Library

from setuptools import setup, Extension
import re
import os


def get_version() -> str:
    """Get version from the Makefile"""
    with open(os.path.join(ROOT, "Makefile"), "r") as makefile:
        for line in makefile:
            if line.startswith("VERSION"):
                version = line.replace("VERSION", "")
                version = version.replace("=", "").strip()
                return version
    raise RuntimeError("VERSION not found in Makefile")


def insert_version(version: str, file: str):
    """Must be used to insert version inside pylibspot"""
    file_content = open(file, "r").read()
    regex = re.compile(r"__version__ =.*\n")
    new_content = regex.sub(f"__version = {version}\n", file_content)
    with open(file, "w") as w:
        w.write(new_content)


ROOT = os.path.dirname(__file__)
VERSION = get_version()
SRC_DIR = os.path.join(ROOT, "src")
INCLUDE_DIRS = [os.path.join(ROOT, "include")]
SRC = [os.path.join(SRC_DIR, f) for f in os.listdir(SRC_DIR)]

PYTHON_DIR = "python"
PY_MODULES = ["pylibspot"]

insert_version(VERSION, os.path.join(PYTHON_DIR, f"{PY_MODULES[0]}.py"))

# Extension to build the library
libspot = Extension(
    "libspot",
    sources=SRC,
    include_dirs=INCLUDE_DIRS,
    define_macros=[("VERSION", f'"{VERSION}"')],
    language="c++",
    extra_compile_args=[
        "-pedantic",
    ],
    extra_link_args=[
        "-static-libgcc",
        "-static-libstdc++",
    ],
)

setup(
    name=PY_MODULES[0],
    package_dir={"": PYTHON_DIR},
    py_modules=PY_MODULES,
    version=VERSION,
    description="Python interface to libspot",
    ext_modules=[libspot],
    zip_safe=False,
)
