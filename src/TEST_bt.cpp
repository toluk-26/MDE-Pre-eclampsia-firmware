/**
 * @file TEST_bt.cpp
 * @brief test bluetooth functionality. be sure to use env:test_bt
 * @author Tolu Kolade
 * @date December 23, 2025
 */

#include "bt.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <bluefruit.h>

#ifndef DEBUG
#error "you seem to be in the wrong env"
#endif

PESBt bt;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < SERIAL_WAIT_TIME) {
        // wait for serial to connect, for up to 10 seconds (10000 milliseconds)
    }

    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("        BLE Program");
    Serial.println("-----------------------------\n");

    bt.init();
}

void loop() {}