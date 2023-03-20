import os
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Union

from generated import (
    Briefdescription,
    Compounddef,
    Detaileddescription,
    Location,
    Memberdef,
    Param,
    Parameteritem,
    Parameterlist,
    Ref,
    Sectiondef,
    TypeType,
)
from generated.doxygen import Doxygen
from generated.doxygenindex import Doxygenindex
from pydantic import BaseModel  # pylint: disable=no-name-in-module
from xsdata.formats.dataclass.parsers import XmlParser


def parse_doxygen_folder(
    folder: Union[str, Path]
) -> Tuple[Doxygenindex, Dict[str, Compounddef], Dict[str, Memberdef]]:
    if isinstance(folder, str):
        folder = Path(folder)
    parser = XmlParser()

    index = Doxygenindex()

    compounds: Dict[str, Compounddef] = {}
    members: Dict[str, Memberdef] = {}

    for file in os.listdir(folder):
        if not file.endswith(".xml"):
            continue

        if file == "index.xml":
            index: Doxygenindex = parser.from_path(folder / file, Doxygenindex)
        else:
            d: Doxygen = parser.from_path(folder / file, Doxygen)
            if d.compounddef and d.compounddef.id:
                compounds[d.compounddef.id] = d.compounddef
                for s in d.compounddef.sectiondef:
                    for m in s.memberdef:
                        if m.id:
                            members[m.id] = m
    return index, compounds, members


def c_to_py_type(c_type: str) -> str:
    c_type = (
        c_type.replace("const", "")
        .replace("struct", "")
        .replace("(", " ")
        .strip()
    )

    if "enum" in c_type:
        return "c_int"

    # pointer case
    if c_type.endswith("*"):
        base = c_type[:-1].strip()
        if base == "void":
            return "c_void_p"
        elif base == "char":
            return "c_char_p"
        return f"POINTER({c_to_py_type(base)})"

    return {
        "int": "c_int",
        "char": "c_char",
        "double": "c_double",
        "unsigned long": "c_ulong",
        "void": "None",
    }.get(c_type, c_type)


class Parameter(BaseModel):
    name: Optional[str] = None
    type: str
    description: Optional[str] = None

    def c_prototype(self) -> str:
        return f"{self.type} {self.name}"

    def py_type(self) -> str:
        return c_to_py_type(self.type)

    def md_row(self) -> str:
        return f"| `{self.name}` | `#!c {self.type}` | {self.description} |"

    def __hash__(self) -> int:
        stream = ""
        if self.name:
            stream += self.name
        if self.type:
            stream += self.type
        if self.description:
            stream += self.description
        return hash(stream)


class Structure(BaseModel):
    file: Optional[str]
    name: str
    attributes: List[Parameter]
    description: Optional[str]

    def __gt__(self, other: "Structure") -> bool:
        key = f"struct {other.name.capitalize()}"
        for p in self.attributes:
            if p.type == key:
                return True
        return False

    def __lt__(self, other: "Structure") -> bool:
        return other.__gt__(self)

    def __hash__(self) -> int:
        stream = ""
        if self.file:
            stream += self.file
        if self.name:
            stream += self.name
        if self.description:
            stream += self.description

        return hash(stream) + sum([hash(p) for p in self.attributes])

    def c_prototype(self) -> str:
        attrs = "\n".join(
            [f"{' ' * 4}{p.c_prototype()};" for p in self.attributes]
        )
        return f"""
struct {self.name} {{
{attrs}
}};""".strip(
            " \n"
        )

    def py_binding(self) -> str:
        fields = "\n".join(
            [
                f"""{' ' * 8}("{p.name}", {p.py_type()}),"""
                for p in self.attributes
            ]
        )
        return f'''
class {self.name}(Structure):
    """{self.description}
    """
    _fields_ = [
{fields}
    ]'''.strip(
            " \n"
        )

    def md(self) -> str:
        rows = "\n    ".join([p.md_row() for p in self.attributes])
        padded_body = f"""
<div id="struct_{self.name}"></div>

```c
{self.c_prototype()}
```
{self.description}

| Parameter | Type | Description |
| --------- | ---- | ----------- |
{rows}
""".replace(
            "\n", "\n    "  # add padding
        )
        return f'??? struct "{self.name}"\n{padded_body}'


class Function(BaseModel):
    file: Optional[str] = None
    name: Optional[str] = None
    return_type: str
    parameters: List[Parameter] = []
    description: Optional[str] = None

    def c_prototype(self, max_size: int = 79) -> str:
        prefix = f"{self.return_type} {self.name}"
        args = ", ".join([p.c_prototype() for p in self.parameters])
        inline = f"{prefix}({args})"
        if len(inline) > max_size:
            padding = " " * (len(prefix) + 1)
            args = f",\n{padding}".join(
                [p.c_prototype() for p in self.parameters]
            )
            return f"{prefix}({args})"
        return inline

    def py_binding(self, prefix: str = "lib") -> str:
        args = [p.py_type() for p in self.parameters]
        return f'''
{prefix}.{self.name}.argtypes = [{', '.join(args)}]
{prefix}.{self.name}.restype = {c_to_py_type(self.return_type)}
{self.name} = {prefix}.{self.name}
{self.name}.__doc__ = """{self.description}"""
'''.strip(
            " \n"
        )

    def md(self) -> str:
        rows = "\n    ".join([p.md_row() for p in self.parameters])
        padded_body = f"""
<div id="fun_{self.name}"></div>

```c
{self.c_prototype()}
```
{self.description}

| Parameter | Type | Description |
| --------- | ---- | ----------- |
{rows}
""".replace(
            "\n", "\n    "  # add padding
        )
        return f'??? fun "{self.name}"\n{padded_body}'


class Typedef(BaseModel):
    file: str
    name: str
    type: Union[Function, Structure, str]
    c_prototype: str
    description: Optional[str] = None

    def py_binding(self) -> str:
        if isinstance(self.type, Function):
            args = [c_to_py_type(self.type.return_type)]
            args += [p.py_type() for p in self.type.parameters]

            return f"""
{self.name} = CFUNCTYPE({', '.join(args)})
""".strip(
                " \n"
            )
        return f"{self.name} = {self.type}"

    def md(self) -> str:
        padded_body = f"""
<div id="typedef_{self.name}"></div>

```c
{self.c_prototype}
```
{self.description}
""".replace(
            "\n", "\n    "  # add padding
        )
        return f'??? fun "{self.name}"\n{padded_body}'


def parse_typedef(m: Memberdef) -> Typedef:
    assert m.kind == "typedef"
    assert m.name is not None
    assert m.definition is not None

    name = m.name
    file = parse_location(m.location)
    description = parse_briefdescription(m.briefdescription)
    c_prototype = m.definition
    t = parse_type(m.type)
    if "(" in t:
        # it means that it is a function
        t = t.replace("(", "").strip("*")
        if m.argsstring:
            args = [
                Parameter(type=a)
                for a in m.argsstring.replace("(", "")
                .replace(")", "")
                .split(",")
            ]
        else:
            args = []
        td = Function(
            return_type=c_to_py_type(t),
            parameters=args,
        )
    else:
        td = t
    return Typedef(
        file=file,
        name=name,
        type=td,
        description=description,
        c_prototype=c_prototype,
    )


def parse_type(t: str | TypeType | None) -> str:
    if t is None:
        return ""
    if isinstance(t, str):
        return t
    return parse_content(t.content)  # type: ignore


def parse_content(content: List[str | Ref]) -> str:
    out = ""
    for c in content:
        if isinstance(c, str):
            out += c
        elif isinstance(c, Ref):
            out += c.value
        else:
            raise TypeError(f"cannot handle type {type(c)}")
    return out


def parse_location(location: Location | None) -> str:
    if location is None:
        return ""
    if location.file is not None:
        return location.file
    return ""


def parse_briefdescription(description: str | Briefdescription | None) -> str:
    if description is None:
        return ""
    if isinstance(description, str):
        return description.replace("\n", "").strip()

    if description.para is None:
        return ""

    if isinstance(description.para, str):
        return description.para

    return parse_content(description.para.content)  #  type: ignore


def parse_param(param: Param) -> Parameter:
    return Parameter(
        name=param.declname,
        type=parse_type(param.type),
        description=None,
    )


def parameter_name(param: Parameteritem) -> str:
    pl = param.parameternamelist
    if pl is None:
        return ""
    if pl.parametername is None:
        return ""
    if isinstance(pl.parametername, str):
        return pl.parametername
    if isinstance(pl.parametername, int):
        return f"{pl.parametername}"
    return pl.parametername.value


def parameter_description(param: Parameteritem) -> str:
    pd = param.parameterdescription
    if pd is None:
        return ""
    if pd.para is None:
        return ""
    if isinstance(pd.para, str):
        return pd.para
    return parse_content(pd.para.content)  # type: ignore


def extract_param_description(
    d: str | Detaileddescription | None,
) -> Dict[str, str]:
    out = {}
    if d is None:
        return out

    if isinstance(d, str):
        return {d: ""}

    for p in d.para:
        if isinstance(p, str):
            continue
        for c in p.content:
            if isinstance(c, Parameterlist):
                if c.kind != "param":
                    continue
                for item in c.parameteritem:
                    name = parameter_name(item).strip()
                    details = parameter_description(item).strip()
                    out[name] = details
    return out


def parse_details(d: str | Detaileddescription | None) -> str:
    if d is None:
        return ""
    if isinstance(d, str):
        return d
    if isinstance(d, Detaileddescription):
        for p in d.para:
            if isinstance(p, str):
                continue
            for c in p.content:
                if isinstance(c, str):
                    return c
    return ""


def parse_function(member: Memberdef) -> Function:
    assert member.kind == "function"

    file = parse_location(member.location)
    name = member.name if member.name else ""
    return_type = parse_type(member.type)
    description = parse_briefdescription(member.briefdescription).strip()
    # add details if they exist
    details = parse_details(member.detaileddescription)
    if len(details) > 0:
        description += f"\n{details}"

    parameters = [parse_param(p) for p in member.param]

    desc = extract_param_description(member.detaileddescription)

    for p in parameters:
        if p.name in desc:
            p.description = desc[p.name]

    return Function(
        file=file,
        name=name,
        return_type=return_type,
        description=description,
        parameters=parameters,
    )


def parse_struct(s: Compounddef) -> Structure:
    assert s.kind == "struct", f"Bad member kind: {s.kind}"
    name = s.compoundname
    description = parse_briefdescription(s.briefdescription).strip()
    file = parse_location(s.location)

    section: Sectiondef = next(
        filter(lambda sd: sd.kind == "public-attrib", s.sectiondef)
    )
    attributes = []
    for member in section.memberdef:
        if member.kind != "variable":
            continue

        attributes.append(
            Parameter(
                name=member.name,
                type=parse_type(member.type),
                description=parse_briefdescription(member.briefdescription),
            )
        )

    return Structure(
        file=file,
        name=name if name else "???",
        attributes=attributes,
        description=description,
    )


def topological_sort(structs: List[Structure]) -> List[Structure]:
    d = {}
    for i, a in enumerate(structs[:-1]):
        for b in structs[(i + 1) :]:
            # c = cmp(a, b)
            if a < b:
                # here a must be before b
                d[b] = d.get(b, []) + [a]
            elif a > b:
                d[a] = d.get(a, []) + [b]

    def kahn(d: dict):
        S = set(structs).difference(d.keys())
        L = []
        while len(S) > 0:
            n = S.pop()
            L.append(n)
            for m, v in d.items():
                # edge from n to m
                if n in v and len(v) == 1:
                    S.add(m)

        return L

    return kahn(d)
