/*
Filename: accelerometer.cpp
Author: Samuel Thompson
Description:        add desc
Date: 2026/02/07
Modified:
*/

#include "accelerometer.hpp"

#include <Arduino.h>

#include "LSM6DS3.h"
#include "Wire.h"

// initializes IMU class, sets preferred application settings for IMU - turning
// off temp and gyro, update accelerometer settings
accelerometer::accelerometer() : IMU(I2C_MODE, 0x6A) {
    IMU.settings.accelEnabled = 1;      // enabled accelerometer
    IMU.settings.accelRange = 2;        // +-2g max range
    IMU.settings.accelSampleRate = 104; // ODR
    IMU.settings.accelBandWidth = 50;   // downsampling anti-aliasing filter Fc

    // disable gyroscope and temperature sensor
    IMU.settings.gyroEnabled = 0;
    IMU.settings.tempEnabled = 0;
}

void accelerometer::start() {
    IMU.settings.accelEnabled = 1;
    IMU.begin();
}

void accelerometer::stop() {
    IMU.settings.accelEnabled = 0;
    IMU.begin();
}

std::vector<float> accelerometer::getxyzg() {
    std::vector<float> xyz;
    xyz.push_back(getxg());
    xyz.push_back(getyg());
    xyz.push_back(getzg());
    return xyz;
}

float accelerometer::getxg() { return IMU.readFloatAccelX(); }

float accelerometer::getyg() { return IMU.readFloatAccelY(); }

float accelerometer::getzg() { return IMU.readFloatAccelZ(); }
