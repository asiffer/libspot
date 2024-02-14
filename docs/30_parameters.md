---
title: Parameters
---

**SPOT** can be very powerful, once the parameters are coherent. This is not about precision mechanics but there is rule of thumbs you can follow.

## Detection

The main parameter is **`q`**. It defines the probability of an abnormal event. For example, if `q = 0.001`, it means that the algorithm will consider events with a probability lower than `0.1%` as anomalies.

The impact of `q` defines the trade-off between the detection rate and the false positive rate. When `q` is low (e.g. `1e-8`), SPOT will flag only very extreme events so the false positive rate will be low (very extreme events are likely to be true anomalies) and so the detection rate (there will be more unflagged anomalies). The revert phenomenon occurs when `q` is "high".

| `q`  | Example | Detection rate | False positive rate |
| ---- | ------- | -------------- | ------------------- |
| low  | `1e-8`  | low            | low                 |
| high | `1e-3`  | high           | high                |

## Bias/variance tradeoff

The parameters `level` and `size` (from the [`spot_fit`](70_API.md#spot_fit) function) are involved in the fit step of the algorithm.

The `size` is the number of data used for calibration while `1-level` represents the proportion of these initial data that belong to the tail of the distribution (`level` is then a high quantile in practice). For example, let us use `size = 1000` and `level = 0.99`. The algorithm will drop the 990 lowest data and will keep the 10 highest to make a first tail fit.

<!-- prettier-ignore -->
!!! Warning
    In practice, the user must ensure that `n_init * (1 - level)` is high enough to perform the fit (at least few dozens of data). See the paragraph below.

When `size` is fixed, the parameter `level` tunes the [bias/variance tradeoff](https://en.wikipedia.org/wiki/Bias%E2%80%93variance_tradeoff).
We have explained that the number of data to perform the fit is `(1-level)*size`. If `level` is high (close to 1), we are more likely to shape the right tail (low bias) but as the number of fitting data is low, the latter will be more variable (high variance). Conversely if `level` is not too close to 1, we will have more data to fit the tail (low variance) but maybe our fit will involve data that do not belong to the tail (high bias).

| `level` | Example | Bias | Variance |
| ------- | ------- | ---- | -------- |
| low     | `0.95`  | high | low      |
| high    | `0.999` | low  | high     |

Ideally, if you have plenty of records, you can take a very high `level` without worrying about the variance.

<!-- prettier-ignore -->
!!! Warning
    Remember also that `1-level` cannot be lower that `q` otherwise it leads to a contradiction between what should be flagged and what should be in the tail.

## Bounded memory

In theory, the number of data in the tail can grow indifinetely while monitoring an infinite stream. Everyone knows that memory resources are limited so we cannot store all of the data to update the tail. Here comes the `max_excess` parameters: it defines the number of tail data we will keep.

Moreover, it creates a memory vanishing effect since the model will keep only the last `max_excess` tail data to perform the fit. Thus, it must be high enough to perform a good fit in terms of bias and variance (see the paragraph above) but beware of the tail dynamics: if you need to quickly adapt to the "new" shape of the tail, `max_excess` should not be too high (one may advice few hundreds but it could be more if the monitored stream is stable).
