---
title: Install
---

If you are only interested in the [python interface](50_python.md) the [dynamic] library is packaged with the python wheel.

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
    By default it install the libraries to `/usr/lib` and headers to `/usr/include/spot`. You can change it through `INSTALL_HEAD_DIR` and `INSTALL_LIB_DIR` variables:
    ```shell
    sudo make install INSTALL_HEAD_DIR=/tmp/spot INSTALL_LIB_DIR=/tmp
    ```
    Or you can use the `DESTDIR` (unset) or `PREFIX` variables (set to `/usr`). Here is the extract of the Makefile:
    ```makefile
    DESTDIR =
    PREFIX = /usr
    INSTALL_HEAD_DIR = $(DESTDIR)$(PREFIX)/include/spot
    INSTALL_LIB_DIR = $(DESTDIR)$(PREFIX)/lib
    ```

You can remove the library with the `uninstall` command.
```shell
sudo make uninstall
```