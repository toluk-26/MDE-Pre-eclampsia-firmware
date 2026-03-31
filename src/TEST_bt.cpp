/**
 * @file TEST_bt.cpp
 * @brief test bluetooth functionality. be sure to use env:test_bt
 * @author Tolu Kolade
 * @date December 23, 2025
 */

#include "StreamController.hpp"
#include "TransferController.hpp"
#include "bt.hpp"
#include "flashlog.hpp"
#include "log.hpp"
#include "rtc.hpp"
#include <Arduino.h>

// #ifndef DEBUG
// #error "you seem to be in the wrong env"
// #endif

TransferController tcrtl;
StreamController sctrl;

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
    // Log::sensor(91, 90, 120, 0);
    // LOGS("TEST0");
    // Log::sensor(92, 92, 123, 0);
    // Log::sensor(93, 90, 120, 0);
    // delay(1000);
    // Log::sensor(94, 92, 123, 0);
    // Log::sensor(91, 90, 120, 0);
    // LOGS("TEST1");
    // LOGS("TEST2");
    // Log::sensor(92, 92, 123, 0);
    // Log::sensor(93, 90, 120, 0);
    // delay(1000);
    // Log::sensor(94, 92, 123, 0);
    // Log::sensor(91, 90, 120, 0);
    // Log::sensor(92, 92, 123, 0);
    // Log::sensor(93, 90, 120, 0);
    // delay(1000);
    // LOGS("TEST3");
    // Log::sensor(94, 92, 123, 0);
    // Log::sensor(91, 90, 120, 0);
    // Log::sensor(92, 92, 123, 0);
    // delay(1000);
    // LOGS("TEST4");
    // Log::sensor(93, 90, 120, 0);
    // delay(1000);
    // Log::sensor(94, 92, 123, 0);
    // LOGS("TEST5");
    // delay(2000);
    // LOGS("TEST6");
    LOGV("Done");
}

void loop() {
    rtc.tick();

    if (bt.transferService.transfer_flag && tcrtl.isDone()) {
        tcrtl.begin();
    }

    if (!tcrtl.isDone()) {
        tcrtl.run();
    }

    if (bt.calibrateService.stream_flag) sctrl.run();
}