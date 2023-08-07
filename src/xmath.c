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

/*
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 */
typedef union {
    double d;
    struct {
        unsigned long long significand : 52;
        unsigned long long exponent : 11;
        unsigned long long sign : 1;
    } bits;
} double_cast;

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
        return 1.0 / _exp(-x);
    }
    if (x > LOG2) {
        unsigned long long k = x / LOG2;
        double r = x - LOG2 * (double)k;
        double_cast w = {.d = _exp_cf_6(r)};
        w.bits.exponent += k;
        return w.d;
    }

    return _exp_cf_6(x);
}

double xlog(double x) {
    double_cast casted = {.d = x};
    if (casted.bits.exponent == 1022 || casted.bits.exponent == 1023) {
        // 1/2 < x < 1
        // when x is close to 1 (but <1)., it is better
        // to call the function directly instead of setting exponent to 1023
        return _log_cf_11(x);
    }
    double exponent = casted.bits.exponent;
    casted.bits.exponent = 1023;
    // when exponent is 1023, 1 <= casted.d <= 2
    // return logd(casted.d, depth) + (exponent - 1023.0) * LOG2;
    return _log_cf_11(casted.d) + (exponent - 1023.0) * LOG2;
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

int is_nan(double x) { return x != x; }
