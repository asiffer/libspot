#include "ubend.h"
#include "unity.h"
#include <stdlib.h>

void test_ubend_init(void) {
    unsigned long const capacity = 5;
    struct Ubend Ubend;
    ubend_init(&Ubend, capacity);
    TEST_ASSERT_EQUAL_UINT(0, Ubend.cursor);
    TEST_ASSERT_EQUAL_UINT(0, Ubend.filled);
    TEST_ASSERT_EQUAL_UINT(capacity, Ubend.capacity);
    TEST_ASSERT_DOUBLE_IS_NAN(Ubend.last_erased_data);
}

void test_ubend_push(void) {
    double const value = 42.0;
    unsigned long const capacity = 10;
    struct Ubend Ubend;
    ubend_init(&Ubend, capacity);

    // insert first values (no data erased)
    for (unsigned long i = 0; i < capacity; ++i) {
        TEST_ASSERT_DOUBLE_IS_NAN(ubend_push(&Ubend, value));
    }
    // now the container is filled so we erase
    // the first value at insertion
    TEST_ASSERT_EQUAL_DOUBLE(value, ubend_push(&Ubend, 0.0));
}

void test_ubend_size(void) {
    double const value = -1.0;
    unsigned long const capacity = 15;
    struct Ubend Ubend;
    ubend_init(&Ubend, capacity);

    TEST_ASSERT_EQUAL_UINT(0, ubend_size(&Ubend));

    for (unsigned long i = 0; i < capacity; ++i) {
        ubend_push(&Ubend, value);
        TEST_ASSERT_EQUAL_UINT(i + 1, ubend_size(&Ubend));
    }

    for (size_t i = 0; i < capacity; ++i) {
        ubend_push(&Ubend, value);
        TEST_ASSERT_EQUAL_UINT(capacity, ubend_size(&Ubend));
    }
}

void test_ubend_free(void) {
    unsigned long const capacity = 15;
    struct Ubend Ubend;
    ubend_init(&Ubend, capacity);
    ubend_free(&Ubend);
    TEST_ASSERT_EQUAL_UINT64(0, Ubend.cursor);
    TEST_ASSERT_EQUAL_UINT64(0, Ubend.capacity);
    TEST_ASSERT_EQUAL_INT(-1, Ubend.filled);
    TEST_ASSERT_DOUBLE_IS_NAN(Ubend.last_erased_data);
}

void setUp(void) { internal_set_allocators(malloc, free); }

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ubend_init);
    RUN_TEST(test_ubend_push);
    RUN_TEST(test_ubend_size);
    RUN_TEST(test_ubend_free);
    return UNITY_END();
}