---
title: SPOT algorithm
summary: How it works in backstage
---

SPOT stands for **S**treaming **P**eaks **O**ver **T**hreshold. It is an adaptation of the POT method (you got it) for streaming data. In a word, this method computes high quantiles that are used as anomaly threshold.

## Cruising regime

Suppose that we already have an anomaly threshold $z$ and a tail threshold $t$.
When a new data $X$ comes, SPOT compares it against $z$ to check if it is an anomaly.
If it's not an anomaly, it compares with $t$ to check if $X$ is in the tail of the distribution. If it is not in the tail, the data is then considered as normal. Otherwise we must update the model: first we store the value (the excess $X-t$ in fact) and then we perform a GPD fit (GPD stands for Generalized Pareto Distribution) to find the optimal parameters $\gamma, \sigma$. From them we can derive a new decision threshold $z$. 

~{spot}(spot.json)

## Initial fit

The natural question is how $z$ and $t$ are computed first? 
A training set is basically required. 
The tail threshold $t$ is computed on this set based on the `level` parameter: it is the corresponding quantile.

~{fit}(fit.json)

A simple way to estimate this quantile would be to compute it emirically. For that we need to sort the data... This method gives the right empirical quantile but it requires to implement a sort algorithm (remember `-nostdlib` +twemoji:grinning-face-with-sweat+).

Actually, we do not need the highest precision quantile given `level`. We roughly want a _high_ value. So, **libspot** uses an approximate quantile algorithm, a.k.a. **P**<sup><b>2</b></sup> (see [https://aakinshin.net/p2-quantile-estimator/](https://aakinshin.net/p2-quantile-estimator/) for a very good description).

The benchmark below shows that it is faster and also that it is quite accurate even with a small initial batch.

/// codexec

    :::c
    --8<-- "benchmark/p2.c"

///

Once $t$ is computed, a Generalied Pareto Distribution (GPD) fit is performed on the tail data. It outputs optimal distribution parameters $\gamma,\sigma$ and we can compute $z$ from that with:
$$
z_q = t + \dfrac{\gamma}{\sigma}\left(\left(\dfrac{q n}{N_t}\right)^{-\gamma}  - 1 \right) 
$$
where $q$ is the anomaly probability, $n$ is the size of the training set and $N_t$ is the number of tail data.
