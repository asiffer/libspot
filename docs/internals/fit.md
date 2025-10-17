---
title: Initial fit
---

The fit steps is required to calibrate the algorithm and compute a first decision threshold.

To compute this _extreme_ threshold, we first need a _high_ threshold, that is basically a high quantile (defined by the `level` parameter). This quantile is used to delimitate the tail of the distribution.

A simple way to estimate this quantile is to compute it emirically from the initial batch. For that we need to sort the data... This method gives the right empirical quantile but it requires to implement a sort algorithm (remember `-nostdlib` :sweat_smile:).

Actually, we do not need the highest precision quantile given `level`. We roughly want a _high_ value. So, **libspot** uses an approximate quantile algorithm, a.k.a. **P**<sup><b>2</b></sup> (see [https://aakinshin.net/p2-quantile-estimator/](https://aakinshin.net/p2-quantile-estimator/) for a very good description).

The benchmark below shows that it is faster and also that it is quite accurate even with a small initial batch.

??? info "benchmark/p2.c"
    ```{.onecompiler lang="c" filename="p2.c" theme="dark" hideTitle=true hideNew=true hideStdin=true hideNewFileOption=true}
        --8<-- "benchmark/p2.c"
    ```
