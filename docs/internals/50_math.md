---
title: Math functions
---

**libspot** does not depend on the standard library, so its math. In particular, the SPOT algorithm needs both the exponential and the natural logarithm.


## Natural logarithm

Several strategies exist to compute `log`. Currently we prefer accuracy against speed so we have compared different methods and parameters so as to be as close as possible as the standard library output (on my laptop :grin: but you can also check it with online compiler). 

Our simple implementation is actually **faster** than the standard library while being **as accurate** as it.

### Continued fraction

We currently use the following continued fraction[^1][^2]:

$$
\log\left(1 + z\right) = \dfrac{2 z}{2 + z + \displaystyle\KK_{m = 1}^{\infty}\left(\frac{-m^2 z^2}{(2 m + 1) (2 + z)}\right)}
$$

where

$$
\KK_{m = 1}^{\infty}\left(\frac{-m^2 z^2}{(2 m + 1) (2 + z)}\right) = -\cfrac{z^2}{3 (2 + z) - \cfrac{4 z^2}{5 (2 + z) - \cfrac{9 z^2}{7 (2 + z) - \cfrac{16 z^2}{9 (2 + z) - ...}}}}
$$

In practice, we truncate this expansion to depth $d$. For instance, the following pyton script leverages `sympy` and the recursive behavior of the continued fraction to
output a truncated version of the expansion.

```python
import sympy as sp

def K(x, d: int, m: int = 1):
    if m == d:
        return 0

    a = -m * m
    b = 2 * m + 1
    return a * x * x / (b * (2 + x) + K(x, d, m + 1))

def log_cf(x, d: int):
    z = x - 1
    return 2 * z / (2 + z + K(z, d))

x = sp.Symbol("x")
print(log_cf(x, d=11))
```

### Truncation

The following code is used so as to find the "best" depth. We basically retrieve the max mantissa error and the relative error. Indeed we leverage the [IEEE754 representation](https://en.wikipedia.org/wiki/Double-precision_floating-point_format) so as to turn the computation $\log(x),  x\in\RR$ into $\log(x),  x\in[1, 2]$.


??? info "benchmark/log_cf_accuracy.c"
    ```{.onecompiler lang="c" filename="log_cf_accuracy.c" theme="dark" hideTitle=true hideNew=true hideStdin=true hideNewFileOption=true}
    --8<-- "benchmark/log_cf_accuracy.c"
    ```


The results on my laptop are presented below. Depths 9, 10 or 11 may be good candidates depending on the accuracy need.

depth | max mantissa error | max relative error
------|--------------------|-------------------
5     |           204513805|       3.275499E-08
6     |             6065190|       9.713924E-10
7     |              179516|       2.875076E-11
8     |                5307|       8.500201E-13
9     |                 157|       2.514663E-14
10    |                   6|       8.008480E-16
11    |                   2|       4.022239E-16
12    |                   2|       4.022239E-16
13    |                   2|       4.022239E-16
14    |                   2|       4.022239E-16


### Speed

Let us present the speed of our implementation. In the following benchmark, we use an "inline" representation of the continued fraction with `depth = 11` (instead of the recursive definitions given above).

We compare computation time of **libspot** vs the standard library (on 10M runs, $\scriptsize 10^{-8}<x<10^{8}$).

??? info "benchmark/log_cf_speed.c"
    ```{.onecompiler lang="c" filename="log_cf_speed.c" theme="dark" hideTitle=true hideNew=true hideStdin=true hideNewFileOption=true}
    --8<-- "benchmark/log_cf_speed.c"
    ```

When we turn on optimization flags (like `-O2`) we see that the **libspot implementation is faster**.


## Exponential

Once again we use a continued fraction to compute exponential. 

### Continued fraction

We currently use the following continued fraction[^3]:


$$
\exp\left(z\right) = 1 + \cfrac{2 z}{2 - z + 2 \displaystyle\KK_{m = 1}^{\infty}\left(\cfrac{a_m z^2}{1}\right)}
$$

with 
$$
a_m = \dfrac{1}{4 (2 m - 1) (2 m + 1)}
$$
so if we expand the continued fraction
$$
\KK_{m = 1}^{\infty}\left(\cfrac{a_m z^2}{1}\right) = \cfrac{z^2 / 12}{1 + \cfrac{z^2 / 60}{1 + \cfrac{z^2 / 140}{1 + \cfrac{z^2 / 252}{1 + \cfrac{z^2 / 396}{1 + ...}}}}}
$$

```python
import sympy as sp

def K(z2, d: int, m: int = 1):
    if m == d:
        return 0

    mm = 2 * m
    a = 4 * (mm - 1) * (mm + 1)
    return z2 / (a * (1 + K(z2, d, m + 1)))

def exp_cf(z, d: int):
    z2 = z * z
    return 1 + 2 * z / (2 - z + 2 * K(z2, d, 1))

z = sp.Symbol("z")
print(exp_cf(z, d=6))
```

### Truncation

The following benchmark leads us to choose `d = 6`.

??? info "benchmark/exp_cf_accuracy.c"
    ```{.onecompiler lang="c" filename="exp_cf_accuracy.c" theme="dark" hideTitle=true hideNew=true hideStdin=true hideNewFileOption=true}
    --8<-- "benchmark/exp_cf_accuracy.c"
    ```

depth | max mantissa error | max relative error
------|--------------------|-------------------
3     |          6998321454|       7.769708E-07
4     |            13281258|       1.014192E-14
5     |               16126|       1.790451E-12
6     |                  81|       1.017038E-14
7     |                  90|       1.017038E-14
8     |                  90|       1.017038E-14
9     |                  90|       1.017038E-14


### Speed

Like in the previous `log` benchmark, we use an "inline" representation of the continued fraction with `depth = 6`. We compare computation time of **libspot** vs the standard library (on 10M runs, $\scriptsize 10^{-8}<x<10^{2}$).

??? info "benchmark/exp_cf_speed.c"
    ```{.onecompiler lang="c" filename="exp_cf_speed.c" theme="dark" hideTitle=true hideNew=true hideStdin=true hideNewFileOption=true}
    --8<-- "benchmark/exp_cf_speed.c"
    ```

Once again, **libspot implementation is faster** when optimization flags are set.


## Power

Finally, we need to compute $x^\alpha$ when $x \ge 0$ and $\alpha \in\RR$. Currently we basically use our implementation of `exp` and `log` as $x^\alpha = \exp\left(\alpha \log x\right)$.

??? info "benchmark/pow_accuracy_speed.c"
    ```{.onecompiler lang="c" filename="pow_accuracy_speed.c" theme="dark" hideTitle=true hideNew=true hideStdin=true hideNewFileOption=true}
    --8<-- "benchmark/pow_accuracy_speed.c"
    ```

This current choice leads to worse performances than the standard library. This function is then likely to be improved in the future.


[^1]: Formula (4.5) p.111
    > Khovanskii, A. N. (1963). The application of continued fractions and their generalizations to problems in approximation theory (p. 144). Groningen: Noordhoff.
[^2]: Formula (11.2.3) p.196
    > Cuyt, A. A., Petersen, V., Verdonk, B., Waadeland, H., & Jones, W. B. (2008). Handbook of continued fractions for special functions. Springer Science & Business Media.
[^3]: Formula (11.1.2) p.194
    > Cuyt, A. A., Petersen, V., Verdonk, B., Waadeland, H., & Jones, W. B. (2008). Handbook of continued fractions for special functions. Springer Science & Business Media.