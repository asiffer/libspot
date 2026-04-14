// basic.c
// BUILD:
// $ make
// $ cc -o /tmp/basic examples/basic.c dist/libspot.a* -Idist/ -lm
// RUN:
// $ /tmp/basic [--stdlib]

#include "spot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EXCESS 200

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

unsigned long const TRAIN_SIZE = 20000;
unsigned long const TEST_SIZE = 500000;

// N(0, 1) - Box-Muller transform
double gaussian_random() {
    double u = 1.0 - ((double)rand() / (double)RAND_MAX);
    double v = (double)rand() / (double)RAND_MAX;
    return sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
}

int main(int argc, const char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--stdlib") == 0) {
        set_math_functions(log, exp, pow);
    }

    double buffer[MAX_EXCESS];

    // set random seed
    srand(time(NULL));
    // stack allocation
    struct Spot spot;
    int status = 0;
    // init the structure with some parameters
    status = spot_init(
        &spot,
        1e-4,   // q: anomaly probability
        0,      // low: observe upper tail
        1,      // discard_anomalies: flag anomalies
        0.99,   // level: tail quantile (the 1% higher values shapes the tail)
        buffer, // backing array to store the excesses (tail data)
        MAX_EXCESS // max_excess: size of the backing array (number of
                   // elements)
    );

    if (status < 0) {
        return -status;
    }

    // initial data
    double train[TRAIN_SIZE];
    for (unsigned long i = 0; i < TRAIN_SIZE; i++) {
        train[i] = gaussian_random();
    }

    // fit
    status = spot_fit(&spot, train, TRAIN_SIZE);
    if (status < 0) {
        return -status;
    }

    // run
    double test[TEST_SIZE];
    for (unsigned long i = 0; i < TEST_SIZE; i++) {
        test[i] = gaussian_random();
    }
    int normal = 0;
    int excess = 0;
    int anomaly = 0;

    clock_t start = clock();
    for (unsigned long k = 0; k < TEST_SIZE; k++) {
        switch (spot_step(&spot, test[k])) {
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

    double elapsed = (double)(end - start) / (double)CLOCKS_PER_SEC;

    printf("Time: %.2f ms\n", 1000.0 * elapsed);
    printf("Throughput: %.2f value/s\n", (double)TEST_SIZE / elapsed);
    printf("ANOMALY: %d, EXCESS: %d, NORMAL: %d\n", anomaly, excess, normal);
    return 0;
}
