#include <Arduino.h>
#include <unity.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void makeTrue(){
    TEST_ASSERT(true);
}
void makeFalse(){
    TEST_ASSERT(false);
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(makeTrue);
    RUN_TEST(makeFalse);
    return UNITY_END();
}

/**
 * For Arduino framework
 */
void setup() {
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    runUnityTests();
}
void loop() {}