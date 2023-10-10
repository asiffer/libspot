#include "tail.h"
#include "test_tail_fit.h"
#include "unity.h"
#include <stdlib.h>

static char buffer[256];

void test_tail_init(void) {
    struct Tail Tail;
    unsigned long size = 10;
    tail_init(&Tail, size);

    TEST_ASSERT_DOUBLE_IS_NAN(Tail.gamma);
    TEST_ASSERT_DOUBLE_IS_NAN(Tail.sigma);
}

void test_tail_push(void) {
    struct Tail Tail;
    unsigned long const size = 10;
    tail_init(&Tail, size);

    double const x = 0.0;

    for (unsigned long i = 0; i < 2 * size; ++i) {
        tail_push(&Tail, x);
        // TEST_ASSERT_EQUAL_UINT64(i + 1, Tail.Nt);
    }
}

void test_tail_fit(void) {
    unsigned long size = 0;
    struct Result *R;
    for (unsigned long k = 0; k < N; ++k) {
        R = &results[k];
        struct Tail Tail;
        // size = sizeof(*R.data) / sizeof(double);
        // printf("R.data: %u\n", size);
        tail_init(&Tail, R->size);

        for (unsigned long i = 0; i < R->size; ++i) {
            tail_push(&Tail, R->data[i]);
        }

        double llhood = tail_fit(&Tail);
        sprintf(buffer,
                "gamma=%.6f (%.6f), sigma=%.6f (%.6f), L=%.3f (%.3f) (%s)",
                Tail.gamma, R->gamma, Tail.sigma, R->sigma, llhood, R->llhood,
                R->name);
        // |gamma - gamma_th|/gamma_th < 0.05
        TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(2 * 0.15 * R->gamma, R->gamma,
                                          Tail.gamma, buffer);
        // |sigma - sigma_th|/sigma_th < 0.05
        TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(2 * 0.05 * R->sigma, R->sigma,
                                          Tail.sigma, buffer);
        // either we have a greater likelihood or we are not 5% lower
        TEST_ASSERT_MESSAGE((R->llhood - llhood) / R->llhood < 0.05, buffer);
    }
}

int cmp_double(void const *a, void const *b) {
    double *ad = (double *)a;
    double *bd = (double *)b;
    return (*ad) > (*bd);
}

void push_and_sort(double const *array) {
    for (unsigned long k = 0; k < Nt; ++k) {
        tmp_storage[k] = array[k];
    }
    qsort(tmp_storage, Nt, sizeof(double), cmp_double);
}

// void test_tail_probability(void) {
//     struct Point *point;
//     struct Tail Tail;
//     double p = 0.0;
//     for (unsigned long k = 0; k < npoints; ++k) {
//         point = &points[k];
//         tail.sigma = point->sigma;
//         tail.gamma = point->gamma;
//         p = tail_probability(&Tail, point->q, point->z);
//     }
// }
void test_tail_probability(void) {
    struct Result *R;
    unsigned long const step = 5;
    double const s = (double)Nt / (double)n;
    double const t = 0.0;

    double z = 0.0;
    double p = 0.0;
    double q = 0.0;
    double err = 0.0;
    double err_max = _NAN;

    struct _ {
        double z;
        double p;
        double q;
        double err;
    } status;

    struct Tail Tail;

    for (unsigned long k = 0; k < N; ++k) {
        err_max = _NAN;
        R = &results[k];
        // prepare the tail
        Tail.gamma = R->gamma;
        Tail.sigma = R->sigma;

        // sort the excesses on tmp_storage
        push_and_sort(R->data);
        // loop in the data
        for (unsigned long m = step; m < Nt; m += step) {
            z = tmp_storage[m] - t;
            p = (double)((Nt - m)) / ((double)n);
            q = tail_probability(&Tail, s, z);
            err = fabs(p - q) / p;
            sprintf(buffer, "z=%.8f, p=%.8f, q=%.8f (%s)", z, p, q, R->name);
            // printf("z=%.8f, p=%.8f, q=%.8f err=%.2f %% (%s)\n", z, p, q,
            //        100. * err, R->name);
            TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.60, 0.0, err, buffer);

            if (is_nan(err_max) || (err > err_max)) {
                err_max = err;
                status.err = err;
                status.z = z;
                status.p = p;
                status.q = q;
            }
        }
        printf("[%12s] err_max=%.4f with z=%.6f, p=%.6f, q=%.6f\n", R->name,
               err_max, status.z, status.p, status.q);
    }
}

void test_tail_quantile(void) {
    struct Result *R;
    unsigned long const step = 20;
    double const s = (double)Nt / (double)n;
    double const t = 0.0;

    double z = 0.0;
    double zq = 0.0;
    double q = 0.0;
    double err = 0.0;
    double err_max = _NAN;

    struct _ {
        double z;
        double zq;
        double q;
        double err;
    } status;

    struct Tail Tail;

    for (unsigned long k = 0; k < N; ++k) {
        err_max = _NAN;
        R = &results[k];
        // prepare the tail
        Tail.gamma = R->gamma;
        Tail.sigma = R->sigma;

        // sort the excesses on tmp_storage
        push_and_sort(R->data);
        // loop in the data
        for (unsigned long m = step; m < Nt; m += step) {
            z = tmp_storage[m] - t;
            q = (double)((Nt - m)) / ((double)n);
            zq = tail_quantile(&Tail, s, q);
            err = fabs(z - zq) / z;
            sprintf(buffer, "z=%.8f, zq=%.8f, q=%.8f (%s)", z, zq, q, R->name);
            // printf("z=%.8f, zq=%.8f, q=%.8f err=%.2f %% (%s)\n", z, zq, q,
            //        100. * err, R->name);
            TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.30, 0.0, err, buffer);

            if (is_nan(err_max) || (err > err_max)) {
                err_max = err;
                status.err = err;
                status.z = z;
                status.zq = zq;
                status.q = q;
            }
        }
        printf("[%12s] err_max=%.4f with z=%.6f, zq=%.6f, q=%.6f\n", R->name,
               err_max, status.z, status.zq, status.q);
    }
}

void test_tail_free(void) {
    struct Tail Tail;
    unsigned long size = 10;
    tail_init(&Tail, size);
    Tail.gamma = 0.0;
    Tail.sigma = 1.0;

    tail_free(&Tail);
    TEST_ASSERT_DOUBLE_IS_NAN(Tail.gamma);
    TEST_ASSERT_DOUBLE_IS_NAN(Tail.sigma);
}

void setUp(void) { internal_set_allocators(malloc, free); }

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_tail_init);
    RUN_TEST(test_tail_push);
    RUN_TEST(test_tail_fit);
    RUN_TEST(test_tail_probability);
    RUN_TEST(test_tail_quantile);
    RUN_TEST(test_tail_free);
    return UNITY_END();
}