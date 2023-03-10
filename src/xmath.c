/**
 * @file xmath.c
 * @brief Implementation of math functions
 * @author Alban Siffer (alban.siffer@irisa.fr)
 * @version 2.0a
 * @date Fri Mar 10 09:44:55 AM UTC 2023
 * @copyright GNU General Public License version 3
 *
 */
#include "xmath.h"

static double const LOG2 =
    0.69314718055994528622676398299518041312694549560546875;
static double const E = 2.718281828459045235360287471353;

double const _NAN = 0.0 / 0.0;

double const _INFINITY = +1.0 / 0.0;

/*
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 */
typedef union {
    double d;
    struct {
        unsigned long significand : 52;
        unsigned long exponent : 11;
        unsigned long sign : 1;
    } bits;
} double_cast;

/**
 * @brief compute log(1+x) with taylor expansion (-1 < x < 1)
 *
 * @param x
 * @param i
 * @return double
 */
// static double recursive_taylor_log(double x, unsigned int i)
// {
//     switch (i)
//     {
//     case 0:
//         return 1. / TAYLOR_SIZE;
//     case TAYLOR_SIZE:
//         return x * recursive_taylor_log(x, i - 1);
//     default:
//         return -x * recursive_taylor_log(x, i - 1) + 1 / (TAYLOR_SIZE - i);
//     }
// }

// static double exp_continuous_fraction4(double x, unsigned int depth,
//                                        unsigned int max_depth) {
//     if (depth == max_depth) {
//         return 0.0;
//     }
//     double b = 0.0;
//     if (depth == 1) {
//         b = 1.0;
//     } else if (depth % 2 == 0) {
//         b = -2 * (2 * depth - 1);
//     } else {
//         b = 2 * (2 * depth + 1);
//     }
//     return 1 / (b + exp_continuous_fraction4(x, depth + 1, max_depth));
// }

// See
// https://math.stackexchange.com/questions/18445/fastest-way-to-calculate-ex-up-to-arbitrary-number-of-decimals
/*
static double exp_khovanskii(double x_squared, unsigned int depth,
                             unsigned int max_depth) {
    if (depth == max_depth) {
        return 0.0;
    }
    double b = 2 * (2 * depth + 1);
    return x_squared / (b + exp_khovanskii(x_squared, depth + 1, max_depth));
}*/

static double exp_continuous_fraction2(double x_squared, unsigned int depth,
                                       unsigned int max_depth) {
    if (depth == max_depth) {
        return 0.0;
    }
    double a = 1. / (4. * (2. * depth - 3.) * (2. * depth - 1.));
    return a * x_squared /
           (1 + exp_continuous_fraction2(x_squared, depth + 1, max_depth));
}

// static double exp_continuous_fraction(double x_squared, unsigned int depth,
//                                       unsigned int max_depth) {
//     if (depth == max_depth) {
//         return 2. + 4. * depth;
//     }
//     return 2. + 4. * depth +
//            x_squared /
//                exp_continuous_fraction(x_squared, depth + 1, max_depth);
// }

double xexp(double x) {
    // if (x > 2) {
    //     // return 1.0 / exp(-x);
    //     double t = exp(x / 2.);
    //     return t * t;
    // }
    if (is_nan(x)) {
        return _NAN;
    }
    if (x < 0) {
        return 1.0 / xexp(-x);
    }
    if (x > LOG2) {
        int k = x / LOG2;
        double r = x - LOG2 * (double)k;
        double_cast z = {
            .bits.exponent = k + 1023,
            .bits.significand = 1,
            .bits.sign = 0,
        };
        // return 1.0 / exp(-x);
        // double t = exp(x / 2.);
        return xexp(r) * z.d;
    }
    double x2 = x * x;
    // return 1 / (1 - (2 * x / (2 + x + exp_khovanskii(x2, 1,
    // EXP_CF_DEPTH))));
    return 1 + 2 * x /
                   (2 - x +
                    (1. / 6.) * x2 /
                        (1 + exp_continuous_fraction2(x2, 3, EXP_CF_DEPTH)));
}

// static double log_halley(double x, unsigned int n)
// {
//     double yn = x;
//     double eyn = exp(yn);
//     for (unsigned int i = 0; i < n; ++i)
//     {
//         yn += 2 * (x - eyn) / (x + eyn);
//         // yn += -1 + x / eyn;
//         eyn = exp(yn);
//     }
//     return eyn;
// }

// see Shank's algorithm
// https://www.ams.org/journals/mcom/1954-08-046/S0025-5718-1954-0061464-9/S0025-5718-1954-0061464-9.pdf
/*static double log_shanks(double a_prev, double a, unsigned int depth) {
    if (depth == 0) {
        return 0.0;
    }
    // power ni
    unsigned int power = 0;
    // a^ni
    double a_power = a;
    // temp variable
    // it corresponds to a^(ni + 1) at the end of the loop
    double a_temp = a;
    while (a_temp < a_prev) {
        // keep last power in memory
        a_power = a_temp;
        // compute the next power
        a_temp *= a;
        // increment the power
        ++power;
    }
    return 1.0 / (power + log_shanks(a, a_prev / a_power, depth - 1));
}*/

static double log_cf(double x, double x_squared, unsigned int depth,
                     unsigned int max_depth) {
    if (depth == max_depth) {
        return 0.0;
    }
    double a = -(depth - 1.) * (depth - 1.);
    double b = (2 * depth - 1);
    return a * x_squared /
           (b * (2. + x) + log_cf(x, x_squared, depth + 1, max_depth));
}

double xlog(double x) {
    if (is_nan(x)) {
        return _NAN;
    }
    if (x < 0) {
        return _NAN;
    }
    if (x == 0) {
        return -_INFINITY;
    }
    if (x < 1) {
        return -xlog(1.0 / x);
    }
    if (x == 1) {
        return 0.0;
    }
    if (x > E) {
        double_cast casted = {.d = x};
        long exponent = casted.bits.exponent;
        casted.bits.exponent = 1023;
        return xlog(casted.d) + (exponent - 1023) * LOG2;
    }
    // here 1 < x < E (requested to use Shank's algorithm)
    // return log_shanks(E, x, LOG_SHANKS_DEPTH);
    // x = 1+z
    double z = x - 1;
    double z2 = z * z;
    return 2 * z / (2 + z + log_cf(z, z2, 2, LOG_CF_DEPTH));
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

/*
int is_inf(double x) {
    double_cast posinf = {.bits = {
                              .significand = 0,
                              .exponent = 2047, // 1 bits
                              .sign = 0,
                          }};
    return (x == posinf.d) || (x == -posinf.d);
}*/

/*
double log(double x)
{
    if (x > 10.)
    {
        double_cast casted = {.d = x};
        unsigned long exponent = casted.parts.exponent;
        casted.parts.exponent = 1023;
        return log_halley(casted.d, 30) + (exponent - 1023) * LOG2;
        // return -log_halley(1. / x, 30);
    }
    return log_halley(x, 30);
*/
/*
if (x < 0.0)
{
    return NAN;
}
if (x == 0.0)
{
    return -INFINITY;
}
if (x == 1.0)
{
    return 0.;
}
if (x == 2.0)
{
    return LOG2;
}

if (x > 2.0)
{
    double_cast casted = {.d = x};
    unsigned long exponent = casted.parts.exponent;
    casted.parts.exponent = 1023;
    return log(casted.d) + (exponent - 1023) * LOG2;
}
// 0 < x < 2 (so -1 < x - 1 < 1)
// return recursive_taylor_log(x - 1, TAYLOR_SIZE);
return log_halley(x, 50);*/
// }

// double log(double x)
// {
//     return log_halley(x, 200);
// }
