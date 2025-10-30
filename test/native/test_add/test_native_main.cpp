#include "my_math.hpp"
#include "unity.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_functions1() {
    // test stuff
    for (int i = 0; i <= 10; i++)
        for (int j = 15; j >= -5; j--)
            TEST_ASSERT(i + j == i + j);
    TEST_ASSERT(true);
    TEST_ASSERT(false);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_functions1);
    return UNITY_END();
}