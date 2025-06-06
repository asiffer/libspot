---
title: Overview
---

**libspot** is a `C` library to flag outliers in high-throughput streaming data.

## Goal

**libspot** implements the SPOT algorithm. The latter is the result of [research work](60_about.md) that aims to detect outliers (extreme events) in streams without distribution assumption. Briefly, it is the poor man's anomaly detector.

It should be used when we have either no information (and no strong assumption) about the data you want to monitor (a kind of _blind_ monitoring).

## What's new?

While the first implementation of **libspot** was in `C++`, this new version has been entirely re-implemented in `C`. Actually OOP is a bit overkill for this task and using `C` may ease integrations (target systems and language interoperability). In addition, some boilerplate code has been removed to keep only the core algorithm. Here are the project specs:

- **`c99` standard**: modern `C` but not too much
- **clean API**: few meaningful functions to run the algorithm
- **`-nostdlib`**: **libspot** does not depend on the standard library
- **decent testing**: remember that there is statistics behind :sweat_smile:

The development of **libspot** tries to follow some principles:

- :computer: **UNIX philosophy**: do one thing and do it well. It implies that the library should not embed code that provides too specific features, extra utilities or additional abstraction layers. In return, the project must also meet high quality: code, API, documentation and testing are the main undertakings.
- :feather: **Lightness**: the library should have a low footprint. Actually the SPOT algorithm is against the flow: far from all the deep models that more and more overwhelm our daily life. Obviously it is also far less powerful :grin: but its use-cases are definitely different. In a word, SPOT is a cheap algorithm that must be able to run on cheap systems.
- :shopping_bags: **Portability/Extensibility**: in relation with the lightness goal, the library should be easily integrable to any ecosystem. This is why **libspot** is developed in `C99` without depending on the standard library. In addition, I would like to provide some bindings that will help developers using the algorithm.
