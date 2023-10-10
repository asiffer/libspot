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

double K(double z2, unsigned int d, unsigned int m) {
    if (m == d) {
        return 0.;
    }
    double mm = 2 * m;
    double a = 1. / (4 * (mm - 1) * (mm + 1));
    return z2 * a / (1 + K(z2, d, m + 1));
}

double exp_cf(double z, unsigned int d) {
    double z2 = z * z;
    return 1 + 2 * z / (2 - z + 2 * K(z2, d, 1));
}

double _exp(double x, unsigned int d) {
    if (x < 0) {
        return 1 / _exp(-x, d);
    }
    if (x > LOG2) {
        unsigned long long k = x / LOG2;
        double r = x - LOG2 * (double)k;
        // significand, exponent, sign
        // double_cast z = {.bits = {0, k + 1023, 0}};

        double_cast w = {.d = _exp(r, d)};
        w.bits.exponent += k;
        return w.d;
        // return _exp(r, d) * z.d;
    }

    return exp_cf(x, d);
}

void accuracy(unsigned int d, unsigned int N, struct accuracy_errors *error) {
    double_cast cast_exp = {.d = 0.0};
    double_cast cast_exp_cf = {.d = 0.0};
    double x;

    // clean the errors
    reset_errors(error);

    // errors
    unsigned long long significand_err = 0;
    double relative_err = 0.0;

    // linspace
    double a = -10;
    double b = 2;
    double eps = (b - a) / (double)(N - 1);
    double u = a;

    for (unsigned int i = 0; i < N; i++) {
        x = pow(10.0, u);
        cast_exp.d = exp(x);
        cast_exp_cf.d = _exp(x, d);

        if (cast_exp.bits.significand >= cast_exp_cf.bits.significand) {
            significand_err =
                cast_exp.bits.significand - cast_exp_cf.bits.significand;
        } else {
            significand_err =
                cast_exp_cf.bits.significand - cast_exp.bits.significand;
        }

        // significand error
        if (significand_err > error->significand) {
            error->significand = significand_err;
            error->x_significand = x;
        }

        // relative error
        if (cast_exp.d != 0.0) {
            relative_err = (cast_exp_cf.d - cast_exp.d) / cast_exp.d;
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
    for (unsigned d = 3; d < 10; d++) {
        accuracy(d, N, &error);
        printf("%-6d|%20llu|%19E\n", d, error.significand, error.relative);
    }
}