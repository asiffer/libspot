#include "brent.h"
#include "unity.h"
#include <math.h>

static double linear(double x, void *extra) {
    double *root = (double *)extra;
    return x - (*root);
}

static double square(double x, void *extra) {
    double *root = (double *)extra;
    return (x - (*root)) * (x + (*root));
}

static double cubic(double x, void *extra) {
    double *root = (double *)extra;
    return x * (x - *root) * (x + *root);
}

static double logarithm(double x, void *extra) {
    double *p = (double *)extra;
    return log(*p + x);
}

static double exponential(double x, void *extra) { return x * exp(x) - 1.0; }

static double noroot(double x, void *extra) {
    double *p = (double *)extra;
    return x * x + (*p) * (*p) + 1.0;
}

void test_brent_linear(void) {
    int found;
    double const a = -5;
    double const b = 5;
    double const root = 1.0;
    void *extra = (void *)&root;
    double const epsilon = 1e-8;

    double x = brent(&found, a, b, linear, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, root, x);
}

void test_brent_square(void) {
    int found = -1;
    double const a = 0;
    double const b = 5;
    double root = 3. + 1. / 3.;
    void *extra = &root;
    double const epsilon = 1e-8;

    double x = 0.0;

    x = brent(&found, a, b, square, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, root, x);

    x = brent(&found, -b, a, square, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, -root, x);
}

void test_brent_cubic(void) {
    int found;
    double const a = 1;
    double const b = 5;
    double const root = 2.0;
    void *extra = (void *)&root;
    double const epsilon = 1e-8;
    double x = 0.0;

    x = brent(&found, a, b, cubic, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, root, x);

    x = brent(&found, -b, -a, cubic, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, -root, x);

    x = brent(&found, -a, a, cubic, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, 0.0, x);

    // here the result depends on the bounds
    x = brent(&found, -b, b, cubic, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, 0.0, x);
}

void test_brent_log(void) {
    double t = 3.0;
    int found;
    double const a = -t + 0.5;
    double const b = t + 1.0;
    void *extra = (void *)(&t);
    double const epsilon = 1e-8;
    double x = 0.0;

    x = brent(&found, a, b, logarithm, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, 1.0 - t, x);
}

void test_brent_exp(void) {
    int found;
    double root = 0.56714329040978384011140178699861280620098114013671875;
    double const a = -3;
    double const b = 3;
    void *extra = NULL;
    double const epsilon = 1e-8;
    double x = 0.0;

    x = brent(&found, a, b, exponential, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_DOUBLE_WITHIN(epsilon, root, x);
}

void test_brent_noroot(void) {
    int found;
    double const a = -10;
    double const b = 10;
    double p = 1.0;
    void *extra = &p;
    double const epsilon = 1e-8;

    brent(&found, a, b, noroot, extra, epsilon);
    TEST_ASSERT_EQUAL_INT(0, found);
}

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_brent_linear);
    RUN_TEST(test_brent_square);
    RUN_TEST(test_brent_cubic);
    RUN_TEST(test_brent_log);
    RUN_TEST(test_brent_exp);
    RUN_TEST(test_brent_noroot);
    return UNITY_END();
}