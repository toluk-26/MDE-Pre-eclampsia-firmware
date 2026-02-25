#include "sensors.h"
#include "clock.hpp"
#include <Arduino.h>

void Sensors::init() {
    Wire.begin();
    imu.begin();
    ppg.begin(Wire);
}

void Sensors::calibrate() {
    // implement calibration
}

bool Sensors::motionOK() {
    float ax = imu.readFloatAccelX();
    float ay = imu.readFloatAccelY();
    float az = imu.readFloatAccelZ();
    return abs(ax) < 0.2 && abs(ay) < 0.2;
}

bool Sensors::positionOK() {
    float ax = imu.readFloatAccelX();
    float ay = imu.readFloatAccelY();
    float az = imu.readFloatAccelZ();
    return az > 0.7;
}

bool Sensors::rtcTriggered() { return ::clock.tick(); }

BPStatus Sensors::measureBP() {
    int systolic = random(110, 170);
    int diastolic = random(70, 120);

    if (systolic >= 160 || diastolic >= 110) return BPStatus::CRITICAL;
    if (systolic >= 140 || diastolic >= 90) return BPStatus::ELEVATED;
    return BPStatus::OK;
}