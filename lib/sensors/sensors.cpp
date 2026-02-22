#include "sensors.h"
#include <Arduino.h>

void Sensors::init() {
    Wire.begin();
    imu.begin();
    ppg.begin(Wire);
    rtc.begin();
}

void Sensors::calibrate() {
    // implement calibration
}

bool Sensors::motionOK() {
    float ax, ay, az;
    imu.readAcceleration(ax, ay, az);
    return abs(ax) < 0.2 && abs(ay) < 0.2;
}

bool Sensors::positionOK() {
    float ax, ay, az;
    imu.readAcceleration(ax, ay, az);
    return az > 0.7;
}

bool Sensors::rtcTriggered() {
    // implement wakeup detection
    return true;
}

FSM::BPStatus Sensors::measureBP() {
    int systolic = random(110, 170);
    int diastolic = random(70, 120);

    if (systolic >= 160 || diastolic >= 110) return FSM::BP_CRITICAL;
    if (systolic >= 140 || diastolic >= 90) return FSM::BP_HIGH;
    return FSM::BP_OK;
}