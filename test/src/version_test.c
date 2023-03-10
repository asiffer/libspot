#include "unity.h"
#include "version.h"
#include <stdlib.h>

char buffer[64];

void test_libspot_version(void) {
    libspot_version(buffer);
    TEST_ASSERT_EQUAL_STRING(buffer, VERSION);
}

void test_libspot_license(void) {
    libspot_license(buffer);
    TEST_ASSERT_EQUAL_STRING(buffer, LICENSE);
}

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_libspot_version);
    RUN_TEST(test_libspot_license);
    return UNITY_END();
}