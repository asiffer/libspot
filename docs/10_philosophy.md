---
title: Philosophy
---

The development of **libspot** tries to follow some principles:

- :computer: **UNIX philosophy**: do one thing and do it well. It implies that the library should not embed code that provides too specific features, extra utilities or additional abstraction layers. In return, the project must also meet high quality: code, API, documentation and testing are the main undertakings.
- :feather: **Lightness**: the library should have a low footprint. Actually the SPOT algorithm is against the flow: far from all the deep models that more and more overwhelm our daily life. Obviously it is also far less powerful :grin: but its use-cases are definitely different. In a word, SPOT is a cheap algorithm that must be able to run on cheap systems.
- :shopping_bags: **Portability/Extensibility**: in relation with the lightness goal, the library should be easily integrable to any ecosystem. This is why **libspot** is developed in `C99` without depending on the standard library. In addition, I would like to provide some bindings that will help developers using the algorithm.
