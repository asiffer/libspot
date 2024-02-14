#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SEED 0

double const _NAN = 0.0 / 0.0;
double const DMAX = RAND_MAX;
double const LOG2 = 0x1.62e42fefa39efp-1;
double const CPS = CLOCKS_PER_SEC;

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

double runif() { return (double)rand() / DMAX; }

struct accuracy_errors {
    unsigned long long significand;
    double x_significand;
    double relative;
    double x_relative;
};

struct speeds {
    double stdlib;
    double libspot;
};

void reset_errors(struct accuracy_errors *error) {
    // clean the structure
    error->significand = 0;
    error->relative = 0.0;
    error->x_relative = _NAN;
    error->x_significand = _NAN;
}

void reset_speeds(struct speeds *s) {
    s->libspot = 0.0;
    s->stdlib = 0.0;
};

double _log_cf_11(double z) {
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

double _exp(double x) {
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

double _log(double x) {
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

double _pow(double x, double alpha) { return _exp(alpha * _log(x)); }

double _powbis(double x, double alpha) {
    double_cast casted = {.d = x};
    double p = alpha * (casted.bits.exponent - 1023);
    long long k = p;
    double beta = p - (double)k;
    casted.bits.exponent = 1023;
    double_cast t = {.d = _pow(casted.d, alpha) * _pow(2, beta)};
    t.bits.exponent += k;
    return t.d;
}

void accuracy(double alpha, unsigned int N, struct accuracy_errors *error) {
    double_cast cast_pow = {.d = 0.0};
    double_cast cast_pow_cf = {.d = 0.0};
    double x;

    // clean the errors
    reset_errors(error);

    // errors
    unsigned long long significand_err = 0;
    double relative_err = 0.0;

    // linspace
    double a = -1.;
    double b = 1.;
    double eps = (b - a) / (double)(N - 1);
    double u = a;

    for (unsigned int i = 0; i < N; i++) {
        x = pow(10., u);
        cast_pow.d = pow(x, alpha);
        cast_pow_cf.d = _pow(x, alpha);

        if (cast_pow.bits.significand >= cast_pow_cf.bits.significand) {
            significand_err =
                cast_pow.bits.significand - cast_pow_cf.bits.significand;
        } else {
            significand_err =
                cast_pow_cf.bits.significand - cast_pow.bits.significand;
        }

        // significand error
        if (significand_err > error->significand) {
            error->significand = significand_err;
            error->x_significand = x;
        }

        // relative error
        if (cast_pow.d != 0.0) {
            relative_err = (cast_pow_cf.d - cast_pow.d) / cast_pow.d;
            if (relative_err > error->relative) {
                error->relative = relative_err;
                error->x_relative = x;
            }
        }

        u += eps;
    }
}

void speed(double alpha, unsigned int N, struct speeds *s) {
    unsigned long long reference = 0;
    unsigned long long custom = 0;
    double p = 0.0;
    clock_t start;

    reset_speeds(s);

    srand(SEED);
    start = clock();
    for (unsigned int k = 0; k < N; k++) {
        p = pow(10., 10. * runif() - 8.);
        pow(p, alpha);
    }
    reference = clock() - start;

    // reference
    srand(SEED);
    start = clock();
    for (unsigned int k = 0; k < N; k++) {
        p = pow(10., 10. * runif() - 8.);
        _pow(p, alpha);
    }
    custom = clock() - start;

    s->libspot = (double)custom / CPS;
    s->stdlib = (double)reference / CPS;
}

int main() {
    unsigned int const N = 1000000;
    struct accuracy_errors error;
    struct speeds s;

    srand(SEED);

    double alpha[] = {
        pow(10., -3. + runif()), pow(10., -2. + runif()),
        pow(10., -1. + runif()), pow(10., runif()),
        pow(10., 1. + runif()),
    };

    const size_t A = sizeof(alpha) / sizeof(double);

    printf("ACCURACY ===\n");
    printf("  alpha  | max mantissa error | max relative error\n");
    printf("---------|--------------------|-------------------\n");

    for (size_t i = 0; i < A; i++) {
        accuracy(alpha[i], N, &error);
        printf("%-9.5f|%20llu|%19E\n", alpha[i], error.significand,
               error.relative);
    }

    printf("\nSPEED ===\n");
    printf("  alpha  | libspot |  stdlib | stdlib/libspot \n");
    printf("---------|---------|---------|----------------\n");
    for (size_t i = 0; i < A; i++) {
        speed(alpha[i], N, &s);
        printf("%-9.5f|%9.4f|%9.4f|%16.2f\n", alpha[i], s.libspot, s.stdlib,
               s.stdlib / s.libspot);
    }
}