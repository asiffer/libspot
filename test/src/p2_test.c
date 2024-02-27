#include "p2.h"
#include "unity.h"
#include <math.h>
#include <stdlib.h>

#define SIZE 5000

static char buffer[256];

// declaration of sort5
void sort5(double arr[5]);

double const PI = 0x1.921fb54442d18p+1;
double const _NAN = 0.0 / 0.0;
double const DMAX = RAND_MAX;

static double DATA[SIZE];
static double SORTED_DATA[SIZE];

unsigned int const SEEDS[] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
    35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
    69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
    86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100};

double const PROBABILITIES[] = {
    0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1,
    0.9,  0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99,
};

size_t const N_REP = sizeof(SEEDS) / sizeof(unsigned int);

size_t const Q = sizeof(PROBABILITIES) / sizeof(double);

static int cmp(const void *a, const void *b) {
    if (*(double *)a > *(double *)b)
        return 1;
    else if (*(double *)a < *(double *)b)
        return -1;
    else
        return 0;
}

// U(0, 1)
double runif() { return (double)rand() / DMAX; }

double rgauss() { return sqrt(-2 * log(runif())) * cos(2 * PI * runif()); }

void fill_runif() {
    for (unsigned long i = 0; i < SIZE; i++) {
        DATA[i] = runif();
        SORTED_DATA[i] = DATA[i];
    }
    qsort(SORTED_DATA, SIZE, sizeof(double), cmp);
}

void fill_rgauss() {
    for (unsigned long i = 0; i < SIZE; i++) {
        DATA[i] = rgauss();
        SORTED_DATA[i] = DATA[i];
    }
    qsort(SORTED_DATA, SIZE, sizeof(double), cmp);
}

void test_sort5(void) {
    double actual[] = {5.0, 3.0, 4.0, 1.0, 2.0};
    double expected[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    sort5(actual);
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, 5);

    fill_rgauss();
    for (int i = 0; i < 5; i++) {
        actual[i] = DATA[i];
        expected[i] = DATA[i];
    }
    qsort(expected, 5, sizeof(double), cmp);
    sort5(actual);
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, 5);
}

void test_p2_unif(void) {
    double q = 0.0;
    double q_th = 0.0;
    unsigned long k = 0;

    double max_err = 0.0;
    double err = 0.0;
    double ferr = 0.0;
    double mean_err = 0.0;

    for (size_t i = 0; i < N_REP; i++) {
        srand(SEEDS[i]);
        fill_runif();

        for (size_t j = 0; j < Q; j++) {
            q = p2_quantile(PROBABILITIES[j], DATA, SIZE);
            k = (unsigned long)(PROBABILITIES[j] * SIZE);
            q_th = SORTED_DATA[k];

            err = (q - q_th);
            ferr = fabs(err);
            mean_err += ferr;
            if (ferr > max_err) {
                max_err = ferr;
            }
            if (ferr > 0.01) {
                sprintf(buffer, "SEED: %d - P: %.2f - |ERR|: %.6f", SEEDS[i],
                        PROBABILITIES[j], ferr);
                TEST_MESSAGE(buffer);
            }
            TEST_ASSERT_DOUBLE_WITHIN(0.006, 0.0, err);
        }
    }

    mean_err /= Q * N_REP;
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.0, mean_err);
    sprintf(buffer, "MEAN ERR: %.6f", mean_err);
    TEST_MESSAGE(buffer);
    sprintf(buffer, "MAX ERR: %.6f", max_err);
    TEST_MESSAGE(buffer);
}

void test_p2_gauss(void) {
    double q = 0.0;
    double q_th = 0.0;
    unsigned long k = 0;

    double max_err = 0.0;
    double err = 0.0;
    double ferr = 0.0;
    double mean_err = 0.0;

    for (size_t i = 0; i < N_REP; i++) {
        srand(SEEDS[i]);
        fill_rgauss();

        for (size_t j = 0; j < Q; j++) {
            q = p2_quantile(PROBABILITIES[j], DATA, SIZE);
            k = (unsigned long)(PROBABILITIES[j] * SIZE);
            q_th = SORTED_DATA[k];

            err = (q - q_th) / q_th;
            ferr = fabs(err);
            mean_err += ferr;
            if (ferr > max_err) {
                max_err = ferr;
            }
            if (ferr > 0.1) {
                sprintf(buffer, "SEED: %d - P: %.2f - |ERR|: %.6f", SEEDS[i],
                        PROBABILITIES[j], ferr);
                TEST_MESSAGE(buffer);
            }
            TEST_ASSERT_DOUBLE_WITHIN(0.2, 0.0, err);
        }
    }

    mean_err /= Q * N_REP;
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.0, mean_err);
    sprintf(buffer, "MEAN ERR: %.6f", mean_err);
    TEST_MESSAGE(buffer);
    sprintf(buffer, "MAX ERR: %.6f", max_err);
    TEST_MESSAGE(buffer);
}

void setUp(void) { srand(0); }

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sort5);
    RUN_TEST(test_p2_unif);
    RUN_TEST(test_p2_gauss);
    return UNITY_END();
}