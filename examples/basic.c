// basic.c
// BUILD:
// $ make
// $ cc -o /tmp/basic examples/basic.c -Idist/ -Ldist/ -l:libspot.so.2.0b0 -lm
// RUN:
// $ LD_LIBRARY_PATH=dist /tmp/basic

#include "spot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// U(0, 1)
double runif() { return (double)rand() / (double)RAND_MAX; }

// Exp(1)
double rexp() { return -log(runif()); }

int main(int argc, const char *argv[]) {
    // set random seed
    srand(1);
    // provide allocators to libspot
    set_allocators(malloc, free);
    // stack allocation
    struct Spot spot;
    int status = 0;
    // init the structure with some parameters
    status = spot_init(
        &spot,
        1e-4,  // q: anomaly probability
        0,     // low: observe upper tail
        1,     // discard_anomalies: flag anomalies
        0.998, // level: tail quantile (the 1% higher values shapes the tail)
        200    // max_excess: number of data to keep to summarize the tail
    );

    if (status < 0) {
        return -status;
    }

    // initial data (for the fit)
    unsigned long const N = 20000;
    double initial_data[N];
    for (unsigned long i = 0; i < N; i++) {
        initial_data[i] = rexp();
    }

    // fit
    status = spot_fit(&spot, initial_data, N);
    if (status < 0) {
        return -status;
    }

    // now we can run the algorithm
    int K = 50000000;
    int normal = 0;
    int excess = 0;
    int anomaly = 0;

    clock_t start = clock();
    for (int k = 0; k < K; k++) {
        // rexp();
        switch (spot_step(&spot, rexp())) {
        case ANOMALY:
            anomaly++;
            break;
        case EXCESS:
            excess++;
            break;
        case NORMAL:
            normal++;
            break;
        }
    }
    clock_t end = clock();

    printf("%lf\n", (double)(end - start) / (double)(CLOCKS_PER_SEC));
    printf("ANOMALY=%d EXCESS=%d NORMAL=%d\n", anomaly, excess, normal);
    printf("Z=%.6f T=%.6f\n", spot.anomaly_threshold, spot.excess_threshold);
    return 0;
}
