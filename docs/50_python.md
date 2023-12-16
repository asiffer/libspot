---
title: Python
icon: material/language-python
---

## Install

You can install the python bindings through `pip`. It ships the whole library (so you don't have to install it beforehand).

```shell
pip install libspot
```

The bindings are implemented as a CPython `C` extension (i.e. directly using the CPython API). So the overhead is low.

## Get started

??? info "examples/basic.py"
    ```python
    --8<-- "examples/basic.py"
    ```

![](img/basic.svg)
