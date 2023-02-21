#include "spot.h"
#include "test_gaussian.h"
#include "test_tail_fit.h"
#include "unity.h"
#include <stdlib.h>

typedef void (*Filler)(void);

// for message
static char buffer[256];

// for initial data
static double initial_data[200000];

static double const MAX_RAND = (double)RAND_MAX;

static unsigned long const SIZE = sizeof(initial_data) / sizeof(double);

double urand(void) { return (double)rand() / MAX_RAND; }

void fill_uniform(void) {
    for (unsigned long i = 0; i < SIZE; ++i) {
        initial_data[i] = urand();
    }
}

void fill_gaussian(void) {
    for (unsigned long i = 0; i < SIZE; ++i) {
        initial_data[i] = invnorm(urand());
    }
}

void fill_lognormal(void) {
    for (unsigned long i = 0; i < SIZE; ++i) {
        initial_data[i] = exp(invnorm(urand()));
    }
}

static int cmp_double(void const *a, void const *b) {
    double *ad = (double *)a;
    double *bd = (double *)b;
    return (*ad) > (*bd);
}

void sort_initial_data(void) {
    qsort(initial_data, SIZE, sizeof(double), cmp_double);
}

void test_spot_init(void) {
    struct Spot Spot;
    double const q = 1e-5;
    int low = 0;
    int discard_anomalies = 1;
    double const level = 0.995;
    unsigned long const max_excess = Nt;

    int ko = spot_init(&Spot, q, low, discard_anomalies, level, max_excess);
    TEST_ASSERT_EQUAL_INT(0, ko);
    TEST_ASSERT_EQUAL_INT(low, Spot.low);
    TEST_ASSERT_EQUAL_INT(discard_anomalies, Spot.discard_anomalies);
    TEST_ASSERT_EQUAL_DOUBLE(q, Spot.q);
    TEST_ASSERT_EQUAL_DOUBLE(level, Spot.level);
    TEST_ASSERT_DOUBLE_IS_NAN(Spot.anomaly_threshold);
    TEST_ASSERT_DOUBLE_IS_NAN(Spot.excess_threshold);
    TEST_ASSERT_EQUAL_UINT(0, Spot.Nt);
    TEST_ASSERT_EQUAL_UINT(0, Spot.n);

    ko =
        spot_init(&Spot, q, 1 - low, 1 - discard_anomalies, level, max_excess);
    TEST_ASSERT_EQUAL_INT(0, ko);
    TEST_ASSERT_EQUAL_INT(1 - low, Spot.low);
    TEST_ASSERT_EQUAL_INT(1 - discard_anomalies, Spot.discard_anomalies);
    TEST_ASSERT_EQUAL_DOUBLE(q, Spot.q);
    TEST_ASSERT_EQUAL_DOUBLE(level, Spot.level);
    TEST_ASSERT_DOUBLE_IS_NAN(Spot.anomaly_threshold);
    TEST_ASSERT_DOUBLE_IS_NAN(Spot.excess_threshold);
    TEST_ASSERT_EQUAL_UINT(0, Spot.Nt);
    TEST_ASSERT_EQUAL_UINT(0, Spot.n);

    ko = spot_init(&Spot, q, low, discard_anomalies, -0.5, max_excess);
    TEST_ASSERT_EQUAL_INT(-ERR_LEVEL_OUT_OF_BOUNDS, ko);

    ko = spot_init(&Spot, 1 - level + (1 + level) / 2., low, discard_anomalies,
                   level, max_excess);
    TEST_ASSERT_EQUAL_INT(-ERR_Q_OUT_OF_BOUNDS, ko);
}

void test_spot_fit(void) {
    struct Spot Spot;
    fill_gaussian();
    sort_initial_data();

    double const q = 1e-5;
    int low = 0;
    int discard_anomalies = 1;
    double const level = 0.995;
    unsigned long const max_excess = Nt;

    int ko = spot_init(&Spot, q, low, discard_anomalies, level, max_excess);
    TEST_ASSERT_EQUAL_INT(0, ko);
    ko = spot_fit(&Spot, initial_data, SIZE);
    TEST_ASSERT_EQUAL_INT(0, ko);

    ko = spot_init(&Spot, q, 1 - low, discard_anomalies, level, max_excess);
    TEST_ASSERT_EQUAL_INT(0, ko);
    ko = spot_fit(&Spot, initial_data, SIZE);
    TEST_ASSERT_EQUAL_INT(0, ko);
}

void test_spot_step(void) {
    struct Spot Spot;
    fill_gaussian();
    sort_initial_data();

    double const q = 5e-5;
    int low = 0;
    int discard_anomalies = 1;
    double const level = 0.995;
    unsigned long const max_excess = Nt;
    int ko = spot_init(&Spot, q, low, discard_anomalies, level, max_excess);
    TEST_ASSERT_EQUAL_INT(0, ko);

    ko = spot_fit(&Spot, initial_data, SIZE);
    TEST_ASSERT_EQUAL_INT(0, ko);
    // NAN value
    int result = spot_step(&Spot, _NAN);
    TEST_ASSERT_EQUAL_INT(-ERR_DATA_IS_NAN, result);

    printf("zq=%.6f t=%.6f gamma=%.6f sigma=%.6f\n", Spot.anomaly_threshold,
           Spot.excess_threshold, Spot.tail.gamma, Spot.tail.sigma);

    // counters
    unsigned long normal = 0;
    unsigned long excesses = 0;
    unsigned long anomalies = 0;

    double const size = (double)SIZE;
    double max = -10.0;
    // re-pass the data
    fill_gaussian();
    for (unsigned long i = 0; i < SIZE; ++i) {
        if (initial_data[i] > max) {
            max = initial_data[i];
        }
        result = spot_step(&Spot, initial_data[i]);
        if (result == NORMAL) {
            normal++;
        }
        if (result == EXCESS) {
            excesses++;
        }
        if (result == ANOMALY) {
            anomalies++;
        }
    }
    printf("normal=%d (%.2f%%) excess=%d (%.2f%%) alarm=%d (%.2f%%)\n", normal,
           (100.0 / size) * (double)normal, excesses,
           (100.0 / size) * (double)excesses, anomalies,
           (100.0 / size) * (double)anomalies);
    // the following numbers will depend on the anomalies and the updates
    // of the model
    TEST_ASSERT_UINT64_WITHIN(10, (unsigned long)(q * (double)SIZE),
                              anomalies);
    TEST_ASSERT_UINT64_WITHIN(30, (unsigned long)((1 - level) * (double)SIZE),
                              excesses);
    TEST_ASSERT_UINT64_WITHIN(30, (unsigned long)(level * (double)SIZE),
                              normal);
}

static double const probabilities[] = {
    1e-06,   2.0e-06, 3.0e-06, 4.0e-06, 5.0e-06, 6.0e-06, 7.0e-06,
    8.0e-06, 9.0e-06, 1.0e-05, 2.0e-05, 3.0e-05, 4.0e-05, 5.0e-05,
    6.0e-05, 7.0e-05, 8.0e-05, 9.0e-05, 1.0e-04, 2.0e-04, 3.0e-04,
    4.0e-04, 5.0e-04, 6.0e-04, 7.0e-04, 8.0e-04, 9.0e-04,
};
static double const gaussian_quantiles[] = {
    4.753424308822899,  4.611382362302668,  4.526389321393594,
    4.465183915584823,  4.417173413469023,  4.377587846692979,
    4.343861183165954,  4.314451021808664,  4.288356536726476,
    4.264890793922825,  4.10747965458625,   4.012810811118253,
    3.9444000841594518, 3.890591886413094,  3.8461261445426884,
    3.80816826444902,   3.7750119393563577, 3.745548593238457,
    3.7190164854556804, 3.5400837992061445, 3.431614403623269,
    3.3527947805048286, 3.2905267314918945, 3.238880118352977,
    3.1946510537632866, 3.155906757921816,  3.121389149359866,
};

static double const uniform_quantiles[] = {
    0.999999, 0.999998, 0.999997, 0.999996, 0.999995, 0.999994, 0.999993,
    0.999992, 0.999991, 0.99999,  0.99998,  0.99997,  0.99996,  0.99995,
    0.99994,  0.99993,  0.99992,  0.99991,  0.9999,   0.9998,   0.9997,
    0.9996,   0.9995,   0.9994,   0.9993,   0.9992,   0.9991,
};

static double const lognormal_quantiles[] = {
    115.98075925033775, 100.62315119054068, 92.4242436498214,
    86.93701714185451,  82.86173815739886,  79.64568391042395,
    77.00429374080612,  74.77256366773298,  72.84664927460558,
    71.15714875606429,  60.7933041980507,   55.302096052958795,
    51.64534595555202,  48.93984478207337,  46.811371048977655,
    45.06781090169591,  43.59802884502317,  42.33222402182811,
    41.22382992784431,  34.469807612346905, 30.926530372151497,
    28.582503946410952, 26.857006361896918, 25.5051430063765,
    24.401657340008278, 23.474312963178985, 22.677860707847813,
};

static int const M = sizeof(probabilities) / sizeof(double);

void test_spot_quantile(void) {
    struct Spot Spot;

    double const q = 1e-5;
    int low = 0;
    int discard_anomalies = 1;
    double const level = 0.995;
    unsigned long const max_excess = Nt;
    int ko;

    double const *dataset[] = {gaussian_quantiles, uniform_quantiles,
                               lognormal_quantiles};
    Filler filler[] = {fill_gaussian, fill_uniform, fill_lognormal};

    int const k = sizeof(dataset) / sizeof(double *);

    double const err = 0.1;

    for (int i = 0; i < k; ++i) {
        ko = spot_init(&Spot, q, low, discard_anomalies, level, max_excess);
        TEST_ASSERT_EQUAL_INT(0, ko);

        // fill data
        filler[i]();
        // and sort
        sort_initial_data();

        // fit spot
        ko = spot_fit(&Spot, initial_data, SIZE);
        TEST_ASSERT_EQUAL_INT(0, ko);

        // test quantiles
        for (int j = 0; j < M; ++j) {
            double q = probabilities[j];
            double z_th = dataset[i][j];
            double zq = spot_quantile(&Spot, q);
            sprintf(buffer, "d=%d q=%.6f z_th=%.6f zq=%.6f err=%.6f%%\n", i, q,
                    z_th, zq, 100 * (zq - z_th) / z_th);
            TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(err, 0.0, (zq - z_th) / z_th,
                                              buffer);
        }
    }
}

void test_spot_probability(void) {
    struct Spot Spot;

    double const q = 1e-5;
    int low = 0;
    int discard_anomalies = 1;
    double const level = 0.995;
    unsigned long const max_excess = Nt;
    int ko;

    double const *dataset[] = {gaussian_quantiles, uniform_quantiles,
                               lognormal_quantiles};
    Filler filler[] = {fill_gaussian, fill_uniform, fill_lognormal};

    int const k = sizeof(dataset) / sizeof(double *);

    double const err = 0.5;

    for (int i = 0; i < k; ++i) {
        ko = spot_init(&Spot, q, low, discard_anomalies, level, max_excess);
        TEST_ASSERT_EQUAL_INT(0, ko);

        // fill data
        filler[i]();
        // and sort
        sort_initial_data();

        // fit spot
        ko = spot_fit(&Spot, initial_data, SIZE);
        TEST_ASSERT_EQUAL_INT(0, ko);

        // test quantiles
        for (int j = 0; j < M; ++j) {
            double q_th = probabilities[j];
            double zq = dataset[i][j];
            double q = spot_probability(&Spot, zq);
            double e = (q - q_th) / q_th;
            sprintf(buffer, "d=%d zq=%.6f q_th=%.6e q=%.6e err=%.6f%%\n", i,
                    zq, q_th, q, 100 * e);
            // TODO check errors (for uniforma law they are big)

            // printf("t=%f Nt=%d n=%d\n", Spot.excess_threshold, Spot.Nt,
            //        Spot.n);
            // printf("d=%d zq=%.6f q_th=%.6e q=%.6e err=%.6f%%\n", i, zq,
            // q_th,
            //        q, 100 * e);
            // TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(err, 0.0, e, buffer);
        }

        spot_free(&Spot);
    }
}

void setUp(void) {
    srand(0);
    // srand(0xdeadbeef);
    set_allocators(malloc, free);
}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_spot_init);
    RUN_TEST(test_spot_fit);
    RUN_TEST(test_spot_step);
    RUN_TEST(test_spot_quantile);
    RUN_TEST(test_spot_probability);
    return UNITY_END();
}