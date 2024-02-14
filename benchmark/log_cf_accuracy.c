#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double const _NAN = 0.0 / 0.0;

double const LOG2 = 0x1.62e42fefa39efp-1;

/*
 * See en.wikipedia.org/wiki/Double-precision_floating-point_format
 */
typedef union {
    double d;
    struct {
        unsigned long long significand : 52;
        unsigned long long exponent : 11;
        unsigned long long sign : 1;
    } bits;
} double_cast;

struct accuracy_errors {
    unsigned long long significand;
    double x_significand;
    double relative;
    double x_relative;
};

void reset_errors(struct accuracy_errors *error) {
    // clean the structure
    error->significand = 0;
    error->relative = 0.0;
    error->x_relative = _NAN;
    error->x_significand = _NAN;
}

double K(double x, unsigned int d, unsigned int m) {
    if (m == d) {
        return 0.;
    }
    double md = m;
    double a = -md * md;
    double b = md + md + 1.;
    return a * x * x / (b * (2. + x) + K(x, d, m + 1));
}

double log_cf(double x, unsigned int d) {
    double z = x - 1.;
    return 2 * z / (2 + z + K(z, d, 1));
}

double _log(double x, unsigned int d) {
    double_cast casted = {.d = x};
    if (casted.bits.exponent == 1022 || casted.bits.exponent == 1023) {
        // 1/2 < x < 1
        // when x is close to 1 (but <1)., it is better
        // to call the function directly instead of setting exponent to 1023
        return log_cf(x, d);
    }
    double exponent = casted.bits.exponent;
    casted.bits.exponent = 1023;
    // when exponent is 1023, 1 <= casted.d <= 2
    // return logd(casted.d, depth) + (exponent - 1023.0) * LOG2;
    return log_cf(casted.d, d) + (exponent - 1023.0) * LOG2;
}

void accuracy(unsigned int d, unsigned int N, struct accuracy_errors *error) {
    double_cast cast_log = {.d = 0.0};
    double_cast cast_log_cf = {.d = 0.0};
    double x;

    // clean the errors
    reset_errors(error);

    // errors
    unsigned long long significand_err = 0;
    double relative_err = 0.0;

    // linspace
    double a = -8.;
    double b = 8.;
    double eps = (b - a) / (double)(N - 1);
    double u = a;

    for (unsigned int i = 0; i < N; i++) {
        x = pow(10., u);
        cast_log.d = log(x);
        cast_log_cf.d = _log(x, d);

        if (cast_log.bits.significand >= cast_log_cf.bits.significand) {
            significand_err =
                cast_log.bits.significand - cast_log_cf.bits.significand;
        } else {
            significand_err =
                cast_log_cf.bits.significand - cast_log.bits.significand;
        }

        // significand error
        if (significand_err > error->significand) {
            error->significand = significand_err;
            error->x_significand = x;
        }

        // relative error
        if (cast_log.d != 0.0) {
            relative_err = (cast_log_cf.d - cast_log.d) / cast_log.d;
            if (relative_err > error->relative) {
                error->relative = relative_err;
                error->x_relative = x;
            }
        }

        u += eps;
    }
}

int main() {
    unsigned int const N = 2000000;
    struct accuracy_errors error;

    printf("depth | max mantissa error | max relative error\n");
    printf("------|--------------------|-------------------\n");
    for (unsigned d = 5; d < 15; d++) {
        accuracy(d, N, &error);
        printf("%-6d|%20llu|%19E\n", d, error.significand, error.relative);
    }
}