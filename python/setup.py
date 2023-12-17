import os

from setuptools import Extension, setup

# github action context
GITHUB_ACTION_REPOSITORY = os.environ.get("GITHUB_ACTION_REPOSITORY", None)
if GITHUB_ACTION_REPOSITORY is not None:
    ROOT = os.path.abspath(GITHUB_ACTION_REPOSITORY)
    PWD = os.path.join(ROOT, "python")
else:
    PWD = os.path.dirname(__file__)
    ROOT = os.path.abspath(os.path.join(PWD, ".."))


print("ENVIRON:", os.environ)
print("PWD:", PWD)
print("ROOT:", ROOT)

INCLUDE_DIRS = [os.path.join(ROOT, "include")]
SRC_DIR = os.path.join(ROOT, "src")
SOURCES = [
    os.path.join(ROOT, "src", f)
    for f in os.listdir(os.path.join(ROOT, "src"))
    if f.endswith(".c")
] + [os.path.join(ROOT, "python/libspotmodule.c")]


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


# def modified_cflags() -> str:
#     """Provide custom CFLAGS"""
#     initial = set(sysconfig.get_config_var("CFLAGS").split())
#     initial.discard("-O2")
#     initial.discard("-g")
#     initial.discard("-D_GNU_SOURCE")
#     initial.discard("-Wp,-D_GLIBCXX_ASSERTIONS")
#     initial.add("-O3")
#     initial.add("-Wall")
#     initial.add("-Werror")
#     initial.add("-pedantic")
#     initial.add("-std=c99")
#     return " ".join(initial)


# modify CFLAGS (see above)
# sysconfig._CONFIG_VARS["CFLAGS"] = modified_cflags()  #  type: ignore


# lib = Extension(
#     "libspot",
#     include_dirs=INCLUDE_DIRS,
#     sources=SOURCES,
#     # extra_compile_args=["-Wall", "-pedantic", "-Werror"],
#     extra_link_args=["-nostdlib"],
# )

lib = Extension(
    "libspot",
    language="c",
    include_dirs=INCLUDE_DIRS,
    sources=SOURCES,
    extra_compile_args=["-std=c99"],
    extra_link_args=["-nostdlib"],
    define_macros=[("VERSION", f'"{get_version()}"')],
)


setup(
    name="libspot",
    version=get_version(),
    description="Born to flag outliers (from python)",
    author="Alban Siffer",
    author_email="alban.siffer@irisa.fr",
    url="https://asiffer.github.io/libspot/",
    license="LGPLv3",
    test_suite="test.Test",
    ext_modules=[lib],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Information Technology",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU Lesser General Public License v3 (LGPLv3)",
        "Programming Language :: C",
        "Programming Language :: Python :: 3",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Mathematics",
    ],
)
