#include "allocator.h"
#include "unity.h"
#include <stdlib.h>

void test_allocators(void) {
    internal_set_allocators(malloc, free);
    unsigned long size = 10;
    int *buffer = xmalloc(size * sizeof(int));
    TEST_ASSERT_NOT_NULL(buffer);
    xfree(buffer);
}

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_allocators);
    return UNITY_END();
}