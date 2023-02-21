---
title: Home
---

# Welcome to libspot documentation!

**libspot** is a `C` library to flag outliers in high-throughput streaming data.

## Goal

**libspot** implements the SPOT algorithm. The latter is the result of [research work](/about) that aims to detect outliers (extreme events) in streams without distribution assumption. Briefly, it is the poor man's anomaly detector.

It should be used when we have either no information (and no strong assumption) about the data you want to monitor (a kind of _blind_ monitoring).

## What's new?

While the first implementation of **libspot** was in `C++`, this new version has been entirely re-implemented in `C`. Actually OOP is a bit overkill for this task and using `C` may ease integrations (target systems and language interoperability). In addition, some boilerplate code has been removed to keep only the core algorithm. Here are the project specs:

- **`c99` standard**: modern `C` but not too much
- **clean API**: few meaningful functions to run the algorithm
- **`-nostdlib`**: **libspot** does not depend on the standard library
- **decent testing**: remember that there is statistics behind :sweat_smile:

## Install

```shell
git clone github.com/asiffer/libspot
cd libspot
make
sudo make install
```
