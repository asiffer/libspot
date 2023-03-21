import argparse
import os
from argparse import Namespace
from parser import (
    Structure,
    parse_doxygen_folder,
    parse_function,
    parse_struct,
    parse_typedef,
    topological_sort,
)
from pathlib import Path
from typing import Dict, List, NamedTuple

from black import WriteBack, format_file_in_place
from black.mode import Mode  # pylint: disable=no-name-in-module
from templates import HEADER


class Args(NamedTuple):
    directory: str
    output: str
    prefix: str


def python_cmd(args: Namespace):
    index, compounds, members = parse_doxygen_folder(Path(args.directory))

    with open(args.output, "w", encoding="utf-8") as out:
        out.write(HEADER.format(lib=args.prefix))
        structs: List[Structure] = []
        for _, s in compounds.items():
            if s.kind == "struct":
                structs.append(parse_struct(s))

        # structs.sort(key=cmp_to_key(compare_structs))
        for s in topological_sort(structs):
            # print(s)

            print(s.py_binding(), file=out)

        for m in members.values():
            if m.kind == "typedef":
                td = parse_typedef(m)
                print(td.py_binding(), file=out, end="\n\n")
                # print(format_function_typedef(m), file=out, end="\n\n")

        for m in members.values():
            if m.kind == "function":
                f = parse_function(m)
                print(f.py_binding(args.prefix), file=out, end="\n\n")

    mode = Mode()
    mode.line_length = 79
    format_file_in_place(
        src=Path(args.output),
        fast=False,
        mode=Mode(),
        write_back=WriteBack.YES,
    )


def docs_cmd(args: Namespace):
    _, compounds, members = parse_doxygen_folder(Path(args.directory))

    d: Dict[str, str] = {}

    # structures
    for c in compounds.values():
        if c.kind != "struct":
            continue
        s = parse_struct(c)
        key: str = s.file.removeprefix("include/") if s.file else "???.h"
        d[key] = d.get(key, "") + s.md() + "\n"

    # functions
    for m in members.values():
        if m.kind != "function":
            continue
        # format_function(m)
        f = parse_function(m)
        key: str = f.file.removeprefix("include/") if f.file else "???.h"
        d[key] = d.get(key, "") + f.md() + "\n"

    outdir = Path(args.output)
    if not os.path.exists(outdir):
        os.mkdir(outdir)

    for f, content in d.items():
        with open(outdir / f.replace(".h", ".md"), "w", encoding="utf-8") as w:
            w.write(f"---\ntitle: {f}\n---\n")
            w.write(content)


def test_cmd(args: Namespace):
    _, compounds, members = parse_doxygen_folder(Path(args.directory))

    for c in compounds.values():
        if c.kind != "struct":
            continue
        s = parse_struct(c)
        print(s.py_binding())
    for m in members.values():
        if m.kind != "function":
            continue
        # format_function(m)
        f = parse_function(m)
        print(f.py_binding())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog=os.path.basename(__file__))
    subparsers = parser.add_subparsers(help="sub-command help")

    # generate python code
    python_subparser = subparsers.add_parser("python")
    python_subparser.add_argument(
        "-d",
        "--directory",
        type=str,
        default="./doxygen/xml",
    )
    python_subparser.add_argument(
        "-o",
        "--output",
        type=str,
        default="./python/libspot/libspot.py",
    )
    python_subparser.add_argument(
        "-p",
        "--prefix",
        type=str,
        default="_libspot",
        help="library prefix to use",
    )
    python_subparser.set_defaults(func=python_cmd)

    # generate docs
    docs_subparser = subparsers.add_parser("docs")
    docs_subparser.add_argument(
        "-d",
        "--directory",
        type=str,
        default="./doxygen/xml",
    )
    docs_subparser.add_argument(
        "-o",
        "--output",
        type=str,
        default="./docs/API",
    )
    docs_subparser.set_defaults(func=docs_cmd)

    # test cmd
    test_subparser = subparsers.add_parser("test")
    test_subparser.add_argument(
        "-d",
        "--directory",
        type=str,
        default="./doxygen/xml",
    )
    test_subparser.set_defaults(func=test_cmd)

    # parse
    args = parser.parse_args()
    # dispatch to function
    args.func(args)
