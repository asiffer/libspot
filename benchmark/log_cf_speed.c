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

void speed(unsigned int N) {
    unsigned long long reference = 0;
    unsigned long long custom = 0;
    double p = 0.0;
    clock_t start;

    srand(SEED);
    start = clock();
    for (unsigned int k = 0; k < N; k++) {
        p = pow(10., 20. * (2. * runif() - 1.));
        log(p);
    }
    reference = clock() - start;

    // reference
    srand(SEED);
    start = clock();
    for (unsigned int k = 0; k < N; k++) {
        p = pow(10., 20. * (2. * runif() - 1.));
        _log(p);
    }
    custom = clock() - start;

    //     printf(" STDLIB: %llu\nLIBSPOT: %llu (x%.1f)\n", reference, custom,
    //            (double)custom / (double)reference);
    printf("LIBSPOT: %.3fs\n STDLIB: %.3fs (x%.1f)\n", (double)custom / CPS,
           (double)reference / CPS, (double)reference / (double)custom);
}

int main() {
    unsigned int const N = 10000000;
    speed(N);
}