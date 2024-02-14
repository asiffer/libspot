#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SEED 0

// we use -O2 optimization flag
#pragma GCC optimize("O2")

double const _NAN = 0.0 / 0.0;
double const DMAX = RAND_MAX;
double const LOG2 = 0x1.62e42fefa39efp-1;

double const CPS = CLOCKS_PER_SEC;

// U(0, 1)
double runif() { return (double)rand() / DMAX; }

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

double _exp_cf_6(double z) {
    double z2 = z * z;

    return 2 * z /
               (2 * z2 /
                    (12 * z2 / (z2 * (z2 + 396) / (6 * (z2 + 154)) + 60) +
                     12) -
                z + 2) +
           1;
}

double _exp(double x) {
    if (x > LOG2) {
        unsigned long long k = x / LOG2;
        double r = x - LOG2 * (double)k;
        double_cast w = {.d = _exp_cf_6(r)};
        w.bits.exponent += k;
        return w.d;
    }

    return _exp_cf_6(x);
}

void speed(unsigned int N) {
    unsigned long long reference = 0;
    unsigned long long custom = 0;
    double p = 0.0;
    clock_t start;

    srand(SEED);
    start = clock();
    for (unsigned int k = 0; k < N; k++) {
        p = pow(10., 10. * runif() - 8.);
        exp(p);
    }
    reference = clock() - start;

    // reference
    srand(SEED);
    start = clock();
    for (unsigned int k = 0; k < N; k++) {
        p = pow(10., 10. * runif() - 8.);
        _exp(p);
    }
    custom = clock() - start;

    printf("LIBSPOT: %.3fs\n STDLIB: %.3fs (x%.1f)\n", (double)custom / CPS,
           (double)reference / CPS, (double)reference / (double)custom);
}

int main() {
    unsigned int const N = 10000000;
    speed(N);
}