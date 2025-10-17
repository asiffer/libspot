---
title: Performances
summary: Bindings comparisons
---

Here we show a rough benchmark of the different ways to run **libspot**. With a similar setup (gaussian data) we compare how many inputs the library can treat every second. The absolute values are not so relevant since they may depend on the parameters. 



/// echarts

--8<-- "docs/data/foreign_performances.js"

///

| Parameters   | Value     |
| ------------ | --------- |
| `q`          | `1e-4`    |
| `level`      | `0.99`    |
| `max_excess` | `200`     |
| `#train_set` | `20_000`  |
| `#test_set`  | `500_000` |