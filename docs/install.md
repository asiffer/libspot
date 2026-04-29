---
title: Install
summary: How the get the original C library
---

## Releases

The github repo exposes some [releases](https://github.com/asiffer/libspot/releases/latest) where you can download artifacts.

!!! warning "Important"
    Currently only `amd64` artifacts are built. If you target a different architecture, you must build from sources.

## Build from sources

The classical way is to build from sources.

```shell
git clone github.com/asiffer/libspot
cd libspot
make
```

The `make` command builds both the static and the dynamic libraries. Then to install libs and headers:

```shell
sudo make install
```

!!! info
    By default it install the libraries to `/usr/lib` and header to `/usr/include/spot.h`. You can change it through `INSTALL_HEAD_DIR` and `INSTALL_LIB_DIR` variables:

        :::shell
        sudo make install INSTALL_HEAD_DIR=/tmp/spot INSTALL_LIB_DIR=/tmp

    Or you can use the `DESTDIR` (unset) or `PREFIX` variables (set to `/usr`). Here is the extract of the Makefile:

        :::makefile
        DESTDIR =
        PREFIX = /usr
        INSTALL_HEAD_DIR = $(DESTDIR)$(PREFIX)/include
        INSTALL_LIB_DIR = $(DESTDIR)$(PREFIX)/lib
    

You can remove the library with the `uninstall` command.
```shell
sudo make uninstall
```