/**
 * @file xmath.c
 * @brief Implementation of math functions
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0b
 * @date mar. 21 mars 2023 10:23:09 UTC
 * @copyright GNU Lesser General Public License v3.0
 *
 */
#include "xmath.h"

static double const LOG2 = 0x1.62e42fefa39efp-1;
double const _NAN = 0.0 / 0.0;
double const _INFINITY = +1.0 / 0.0;

// static const double pow2[] = {
//     0x1.0p+0,  0x1.0p+1,  0x1.0p+2,  0x1.0p+3,  0x1.0p+4,  0x1.0p+5,
//     0x1.0p+6,  0x1.0p+7,  0x1.0p+8,  0x1.0p+9,  0x1.0p+10, 0x1.0p+11,
//     0x1.0p+12, 0x1.0p+13, 0x1.0p+14, 0x1.0p+15, 0x1.0p+16, 0x1.0p+17,
//     0x1.0p+18, 0x1.0p+19, 0x1.0p+20, 0x1.0p+21, 0x1.0p+22, 0x1.0p+23,
//     0x1.0p+24, 0x1.0p+25, 0x1.0p+26, 0x1.0p+27, 0x1.0p+28, 0x1.0p+29,
//     0x1.0p+30, 0x1.0p+31, 0x1.0p+32, 0x1.0p+33, 0x1.0p+34, 0x1.0p+35,
//     0x1.0p+36, 0x1.0p+37, 0x1.0p+38, 0x1.0p+39, 0x1.0p+40, 0x1.0p+41,
//     0x1.0p+42, 0x1.0p+43, 0x1.0p+44, 0x1.0p+45, 0x1.0p+46, 0x1.0p+47,
//     0x1.0p+48, 0x1.0p+49, 0x1.0p+50, 0x1.0p+51, 0x1.0p+52, 0x1.0p+53,
//     0x1.0p+54, 0x1.0p+55, 0x1.0p+56, 0x1.0p+57, 0x1.0p+58, 0x1.0p+59,
//     0x1.0p+60, 0x1.0p+61, 0x1.0p+62, 0x1.0p+63, 0x1.0p+64};

// static const int EXP_MAX = sizeof(pow2) / sizeof(double);

// int find_log2_positive_double(double target) {
//     int i = 0;
//     int j = EXP_MAX - 1;

//     if (target < pow2[i] || target > pow2[j]) {
//         // out of bounds
//         return -1;
//     }

//     int m = (i + j) / 2;

//     while ((j - i) > 1) {
//         if (target < pow2[m]) {
//             j = m;
//         } else {
//             i = m;
//         }
//         m = (i + j) / 2;
//     }

//     return i;
// }

// static double _frexp(double x, int *e) {
//     if (x < 0) {
//         return -_frexp(-x, e);
//     }
//     if (x < 1) {
//         int inv_exp = find_log2_positive_double(1.0 / x);
//         *e = -inv_exp + 1;
//         return x * pow2[inv_exp];
//     }
//     int exp = find_log2_positive_double(x);
//     *e = exp;
//     return x / pow2[exp];
// }

// static double ldexp(double x, int n) {
//     if (n < 0 && n > -EXP_MAX) {
//         return x / pow2[-n];
//     }
//     if (n >= 0 && n < EXP_MAX) {
//         return x * pow2[n];
//     }
//     return _NAN;
// }

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

/*
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 */
// typedef union {
//     double d;

//     struct {
//         unsigned int : 32; // mantissa1
//         unsigned int : 20; // mantissa0
//         unsigned int exponent : 11;
//         unsigned int : 1; // sign
//     } bits;
// } double_cast;

int is_nan(double x) { return x != x; }

static double _log_cf_11(double z) {
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

double xexp(double x) {
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

double xlog(double x) {
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

double xpow(double a, double x) { return xexp(x * xlog(a)); }

double xmin(double a, double b) {
    if (is_nan(a) || is_nan(b)) {
        return _NAN;
    }
    if (a < b) {
        return a;
    }
    return b;
}
