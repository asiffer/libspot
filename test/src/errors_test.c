#include "errors.h"
#include "unity.h"
#include <stdlib.h>

void test_error_msg(void) {
    for (enum LibspotError err = ERR_MEMORY_ALLOCATION_FAILED;
         err <= ERR_DATA_IS_NAN; ++err) {
        printf("%s\n", error_msg(err));
    }
}

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_error_msg);
    return UNITY_END();
}