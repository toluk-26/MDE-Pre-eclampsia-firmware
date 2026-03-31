/**
 * @file TEST_bt.cpp
 * @brief test bluetooth functionality. be sure to use env:test_bt
 * @author Tolu Kolade
 * @date December 23, 2025
 */

#include "TransferController.hpp"
#include "bt.hpp"
#include "flashlog.hpp"
#include "rtc.hpp"
#include <Arduino.h>

#ifndef DEBUG
#error "you seem to be in the wrong env"
#endif
#include <append.hpp>

TransferController tcrtl;

void setup() {
    Serial.begin(SERIAL_BAUD);
    while (!Serial && millis() < SERIAL_WAIT_TIME) {
        // wait for serial to connect, for up to 10 seconds (10000 milliseconds)
    }

    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("        BLE Program");
    Serial.println("-----------------------------\n");

    bt.begin();
    mem.begin();
    rtc.setAlarm(rtc.getTime() + 60);

    // mem.cleanLogs();
    // delay(500);
    // LOG_SENSOR(91, 90, 120, 0);
    // LOG_DEBUG("TEST0");
    // LOG_SENSOR(92, 92, 123, 0);
    // LOG_SENSOR(93, 90, 120, 0);
    // delay(1000);
    // LOG_SENSOR(94, 92, 123, 0);
    // LOG_SENSOR(91, 90, 120, 0);
    // LOG_DEBUG("TEST1");
    // LOG_DEBUG("TEST2");
    // LOG_SENSOR(92, 92, 123, 0);
    // LOG_SENSOR(93, 90, 120, 0);
    // delay(1000);
    // LOG_SENSOR(94, 92, 123, 0);
    // LOG_SENSOR(91, 90, 120, 0);
    // LOG_SENSOR(92, 92, 123, 0);
    // LOG_SENSOR(93, 90, 120, 0);
    // delay(1000);
    // LOG_DEBUG("TEST3");
    // LOG_SENSOR(94, 92, 123, 0);
    // LOG_SENSOR(91, 90, 120, 0);
    // LOG_SENSOR(92, 92, 123, 0);
    // delay(1000);
    // LOG_DEBUG("TEST4");
    // LOG_SENSOR(93, 90, 120, 0);
    // delay(1000);
    // LOG_SENSOR(94, 92, 123, 0);
    // LOG_DEBUG("TEST5");
    // delay(2000);
    // LOG_DEBUG("TEST6");
}

void loop() {
    rtc.tick();

    if (bt.transferService.transfer_flag && tcrtl.isDone()) {
        tcrtl.begin();
    }

    if (!tcrtl.isDone()) {
        tcrtl.run();
    }
}