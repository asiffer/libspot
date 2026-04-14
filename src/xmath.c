/**
 * @file xmath.c
 * @brief Implementation of math functions
 * @author Alban Siffer (31479857+asiffer@users.noreply.github.com)
 * @version 3.0a0
 * @date mar. 14 avril 2026 14:59:11 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "xmath.h"

static double const LOG2 = 0x1.62e42fefa39efp-1;
double const _NAN = 0.0 / 0.0;
double const _INFINITY = +1.0 / 0.0;

#if __SIZEOF_DOUBLE__ == 8

typedef union {
    double d;
    __UINT64_TYPE__ i;
} double_cast;

/**
 * @brief frexp implem of musl libc
 * @details see https://git.musl-libc.org/cgit/musl/tree/src/math/frexp.c
 *
 */
static double musl_frexp(double x, int *e) {
    double_cast y = {x};

    int ee = y.i >> 52 & 0x7ff;

    if (!ee) {
        if (x) {
            x = musl_frexp(x * 0x1p64, e);
            *e -= 64;
        } else
            *e = 0;
        return x;
    } else if (ee == 0x7ff) {
        return x;
    }

    *e = ee - 0x3fe;
    y.i &= 0x800fffffffffffffull;
    y.i |= 0x3fe0000000000000ull;
    return y.d;
}

/**
 * @brief ldexp implem of musl libc
 * @details see https://git.musl-libc.org/cgit/musl/tree/src/math/scalbn.c
 * (https://git.musl-libc.org/cgit/musl/tree/src/math/ldexp.c)
 *
 * @param x
 * @param n
 * @return double
 */
static double musl_ldexp(double x, int n) {
    double_cast u = {x};

    // in musl libc, it uses double_t to make aithmetic operations. However it
    // is the same as double once FLT_EVAL_METHOD is 0 (defazult generally) or
    // 1 see https://en.cppreference.com/w/c/numeric/math/float_t double_t y =
    // x;
    double y = x;

    if (n > 1023) {
        y *= 0x1p1023;
        n -= 1023;
        if (n > 1023) {
            y *= 0x1p1023;
            n -= 1023;
            if (n > 1023)
                n = 1023;
        }
    } else if (n < -1022) {
        /* make sure final n < -53 to avoid double
           rounding in the subnormal range */
        y *= 0x1p-1022 * 0x1p53;
        n += 1022 - 53;
        if (n < -1022) {
            y *= 0x1p-1022 * 0x1p53;
            n += 1022 - 53;
            if (n < -1022)
                n = -1022;
        }
    }
    u.i = (__UINT64_TYPE__)(0x3ff + n) << 52;
    x = y * u.d;
    return x;
}

#elif __SIZEOF_DOUBLE__ == 4

typedef union {
    double d;
    __UINT32_TYPE__ i;
} double_cast;

/**
 * @brief frexp implem of musl libc
 * @details see https://git.musl-libc.org/cgit/musl/tree/src/math/frexpf.c
 *
 */
double musl_frexp(double x, int *e) {
    union {
        float f;
        __UINT32_TYPE__ i;
    } y = {x};

    int ee = y.i >> 23 & 0xff;

    if (!ee) {
        if (x) {
            x = frexp(x * 0x1p64, e);
            *e -= 64;
        } else
            *e = 0;
        return x;
    } else if (ee == 0xff) {
        return x;
    }

    *e = ee - 0x7e;
    y.i &= 0x807ffffful;
    y.i |= 0x3f000000ul;
    return y.f;
}

/**
 * @brief frexp implem of musl libc
 * @details see https://git.musl-libc.org/cgit/musl/tree/src/math/scalbnf.c
 *
 */
static double musl_ldexp(double x, int n) {
    double_cast u = {x};

    double y = x;
    // float_t y = x;

    if (n > 127) {
        y *= 0x1p127f;
        n -= 127;
        if (n > 127) {
            y *= 0x1p127f;
            n -= 127;
            if (n > 127)
                n = 127;
        }
    } else if (n < -126) {
        y *= 0x1p-126f * 0x1p24f;
        n += 126 - 24;
        if (n < -126) {
            y *= 0x1p-126f * 0x1p24f;
            n += 126 - 24;
            if (n < -126)
                n = -126;
        }
    }
    u.i = (__UINT32_TYPE__)(0x7f + n) << 23;
    x = y * u.f;
    return x;
}

#elif !defined(USE_CUSTOM_FLOAT_UTILS)
#error                                                                        \
    "Cannot provide ldexp/frexp functions: sizeof(double) is neither 8 nor 4."\
    "You must provide your own functions (-DUSE_CUSTOM_FLOAT_UTILS)"
#endif

/**
 * @brief Default ot musl code
 */
static frexp_fn libspot_frexp = musl_frexp;

/**
 * @brief Default to musl code
 */
static ldexp_fn libspot_ldexp = musl_ldexp;

void internal_set_float_utils(ldexp_fn l, frexp_fn f) {
    if (l) {
        libspot_ldexp = l;
    }
    if (f) {
        libspot_frexp = f;
    }
}

int is_nan(double x) { return x != x; }

static double _log_cf_11(double z) {
    // could be 4x slower
    double x = z - 1;
    double xx = x + 2;
    double x2 = x * x;

    double xx2 = xx + xx;
    double xx3 = xx + xx2;
    double xx5 = xx3 + xx2;
    double xx7 = xx5 + xx2;
    double xx9 = xx7 + xx2;
    double xx11 = xx9 + xx2;
    double xx13 = xx11 + xx2;
    double xx15 = xx13 + xx2;
    double xx17 = xx15 + xx2;
    double xx19 = xx17 + xx2;
    double xx21 = xx19 + xx2;

    return 2 * x /
           (-x2 / (-4 * x2 /
                       (-9 * x2 /
                            (-16 * x2 /
                                 (-25 * x2 /
                                      (-36 * x2 /
                                           (-49 * x2 /
                                                (-64 * x2 /
                                                     (-81 * x2 /
                                                          (-100 * x2 / xx21 +
                                                           xx19) +
                                                      xx17) +
                                                 xx15) +
                                            xx13) +
                                       xx11) +
                                  xx9) +
                             xx7) +
                        xx5) +
                   xx3) +
            xx);
}

double _exp_cf_6(double z) {
    double z2 = z * z;

    return 2 * z /
               (2 * z2 /
                    (12 * z2 /
                         (60 * z2 / (140 * z2 / (7 * z2 / 11 + 252) + 140) +
                          60) +
                     12) -
                z + 2) +
           1;
}

static double nostdlib_exp(double x) {
    if (is_nan(x)) {
        return _NAN;
    }
    if (x < 0) {
        return 1.0 / xexp(-x);
    }
    if (x > LOG2) {
        unsigned int k = x / LOG2;
        double r = x - LOG2 * (double)k;
        return libspot_ldexp(_exp_cf_6(r), k);
        // double_cast w = {.d = _exp_cf_6(r)};
        // w.bits.exponent += k;
        // return w.d;
    }

    return _exp_cf_6(x);
}

static double nostdlib_log(double x) {
    if (x < 0 || is_nan(x)) {
        return _NAN;
    }
    if (x == 0) {
        return -_INFINITY;
    }
    // double_cast casted = {.d = x};
    // if (casted.bits.exponent == 1022 || casted.bits.exponent == 1023) {
    //     // 1/2 < x < 1
    //     // when x is close to 1 (but <1)., it is better
    //     // to call the function directly instead of setting exponent to
    //     // 1023
    //     return _log_cf_11(x);
    // }
    int exponent = 0;
    double mantissa = libspot_frexp(x, &exponent);
    if (exponent == 0 || exponent == -1) {
        return _log_cf_11(x);
    }
    return _log_cf_11(mantissa) + LOG2 * exponent;

    // double exponent = casted.bits.exponent;
    // casted.bits.exponent = 1023;
    // when exponent is 1023, 1 <= casted.d <= 2
    // return logd(casted.d, depth) + (exponent - 1023.0) * LOG2;
    // return _log_cf_11(casted.d) + (exponent - 1023.0) * LOG2;
}

/**
 * @brief Default to nostdlib
 */
static math_fn libspot_log = nostdlib_log;

/**
 * @brief Default to nostdlib
 */
static math_fn libspot_exp = nostdlib_exp;

double xlog(double x) { return libspot_log(x); }

double xexp(double x) { return libspot_exp(x); }

// uses xlog and xexp by default
static double nostdlib_pow(double a, double x) { return xexp(x * xlog(a)); }

/**
 * @brief Default to nostdlib
 */
static math2_fn libspot_pow = nostdlib_pow;

void internal_set_math_functions(math_fn lo, math_fn ex, math2_fn po) {
    if (lo) {
        libspot_log = lo;
    }
    if (ex) {
        libspot_exp = ex;
    }
    if (po) {
        libspot_pow = po;
    }
}

double xpow(double a, double x) { return libspot_pow(a, x); }

double xmin(double a, double b) {
    if (is_nan(a) || is_nan(b)) {
        return _NAN;
    }
    if (a < b) {
        return a;
    }
    return b;
}
