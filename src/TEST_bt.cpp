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

void injectSample();
void injectConfig();

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

    // mem.cleanAll();
    // injectSample();
    // injectConfig();
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

    if (bt.disconnectFlag) {
        bt.configService.saveConfig();
        bt.disconnectFlag = false;
    }
}

void injectConfig() {
    ConfigPack c = {.pid = 12345215,
                    .diastolic_min = 60,
                    .diastolic_max = 80,
                    .diastolic_coeff_m = 1.0f,
                    .diastolic_coeff_b = 0.0f,
                    .systolic_min = 100,
                    .systolic_max = 130,
                    .systolic_coeff_m = 1.0f,
                    .systolic_coeff_b = 0.0f};

    mem.configload = c;
    mem.setConfig();
}

void injectSample() {
    mem.cleanLogs();
    delay(500);

    Log::sensor(62, 118, 72, 0); // 23:05
    // LOGS("Night monitoring started");
    delay(1200);

    Log::sensor(58, 112, 68, 0);
    Log::sensor(61, 115, 70, 0);
    delay(1500);

    Log::sensor(55, 108, 65, 0); // deeper sleep
    LOGS("TEST0");
    delay(1800);

    Log::sensor(64, 120, 74, 0);
    Log::sensor(59, 113, 69, 0);
    delay(1400);

    Log::sensor(57, 110, 67, 0);
    Log::sensor(62, 117, 71, 0);
    LOGS("TEST1");
    delay(1600);

    Log::sensor(60, 114, 70, 0);
    Log::sensor(53, 106, 64, 0); // lowest point
    delay(2000);

    Log::sensor(66, 122, 75, 0); // slight arousal
    LOGS("TEST2");
    delay(1300);

    Log::sensor(59, 112, 68, 0);
    Log::sensor(61, 116, 71, 0);
    Log::sensor(57, 109, 66, 0);
    delay(1700);

    Log::sensor(63, 119, 73, 0);
    LOGS("TEST3");
    delay(1500);

    Log::sensor(58, 111, 67, 0);
    Log::sensor(60, 115, 70, 0);
    delay(1400);

    Log::sensor(55, 107, 65, 0);
    Log::sensor(64, 121, 74, 0);
    delay(1800);

    LOGS("TEST4");
    Log::sensor(59, 113, 69, 0);
    delay(1600);

    Log::sensor(62, 118, 72, 0);
    Log::sensor(56, 109, 66, 0);
    LOGS("TEST5");
    delay(2000);

    Log::sensor(65, 123, 76, 0); // minor movement
    Log::sensor(60, 114, 70, 0);
    delay(1500);

    Log::sensor(58, 111, 68, 0);
    Log::sensor(57, 110, 67, 0);
    delay(1700);

    Log::sensor(63, 119, 73, 0);
    LOGS("TEST6");
    delay(1400);

    Log::sensor(61, 116, 71, 0);
    Log::sensor(54, 108, 65, 0);
    delay(1900);

    Log::sensor(66, 122, 75, 0);
    Log::sensor(59, 113, 69, 0);
    delay(1600);

    Log::sensor(57, 110, 66, 0);
    LOGS("TEST7");
    delay(1500);

    Log::sensor(62, 117, 72, 0);
    Log::sensor(60, 114, 70, 0);
    delay(1800);

    Log::sensor(55, 107, 64, 0); // very calm phase
    Log::sensor(64, 120, 74, 0);
    delay(1700);

    LOGS("TEST8");
    Log::sensor(58, 112, 68, 0);
    delay(1400);

    Log::sensor(61, 115, 71, 0);
    Log::sensor(59, 113, 69, 0);
    delay(2000);

    Log::sensor(63, 119, 73, 0);
    Log::sensor(56, 109, 66, 0);
    delay(1600);

    Log::sensor(65, 121, 75, 0);
    LOGS("TEST9");
    delay(1500);

    Log::sensor(60, 114, 70, 0);
    Log::sensor(58, 111, 67, 0);
    delay(1800);

    Log::sensor(62, 117, 72, 0);
    Log::sensor(57, 110, 66, 0);
    delay(1700);

    LOGS("TEST10");
    Log::sensor(64, 120, 74, 0);
    delay(1400);

    Log::sensor(59, 113, 69, 0);
    Log::sensor(61, 116, 71, 0);
    delay(2000);

    Log::sensor(55, 108, 65, 0);
    LOGS("TEST11");
    delay(1600);

    Log::sensor(66, 123, 76, 0); // final slight rise before waking
    Log::sensor(60, 115, 70, 0);
    delay(1500);

    Log::sensor(58, 112, 68, 0);
    Log::sensor(63, 118, 72, 0);
    delay(1800);

    LOGS("TEST12");
    Log::sensor(61, 116, 71, 0);
    delay(1700);

    Log::sensor(57, 110, 67, 0);
    Log::sensor(59, 113, 69, 0);
    delay(1400);

    Log::sensor(64, 121, 74, 0);
    LOGS("TEST13");
    delay(1600);

    Log::sensor(56, 109, 66, 0);
    Log::sensor(62, 117, 72, 0);
    delay(2000);

    Log::sensor(60, 114, 70, 0);
    LOGV("Night monitoring completed - 50 readings logged");
}