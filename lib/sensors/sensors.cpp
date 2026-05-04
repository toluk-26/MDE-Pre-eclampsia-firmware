#include "sensors.h"
#include "clock.hpp"
#include <Arduino.h>

void Sensors::init() {
    Wire.begin();
    ppg.begin(Wire);
}

void Sensors::calibrate() {
    delay(2000);

    for (int i = 0; i < 100; i++) {
        ppg.getIR();
        delay(10);
    }
}

bool Sensors::motionOK() { return acti.run(); }
bool Sensors::positionOK() { return acti.run(); }
bool Sensors::rtcTriggered() { return ::rtc.tick(); }

BPStatus Sensors::measureBP() {
    int systolic = random(110, 170);
    int diastolic = random(70, 120);

    if (systolic >= 160 || diastolic >= 110) return BPStatus::CRITICAL;
    if (systolic >= 140 || diastolic >= 90) return BPStatus::ELEVATED;
    return BPStatus::OK;
}
