// custom accelerometer wrapper class test

#include "accelerometer.hpp"
#include <vector>

accelerometer xl;

void setup() {
    Serial.begin(SERIAL_BAUD); // startup serial monitor
    while (!Serial) {
        ;
    }
}

void loop() {

    xl.start();
    Serial.println("\naccelerometer ON");
    delay(1000);

    for (int i = 0; i < 6; i++) {
        std::vector<float> xyz = xl.getxyzg();
        Serial.print("X = ");
        Serial.println(xyz[0], 3);
        Serial.print("Y = ");
        Serial.println(xyz[1], 3);
        Serial.print("Z = ");
        Serial.println(xyz[2], 3);
        delay(10); // wait 10ms to avoid printing repeated xl samples
    }
    delay(5000);

    xl.stop();
    Serial.println("\naccelerometer OFF");
    delay(1000);

    for (int i = 0; i < 6; i++) {
        std::vector<float> xyz = xl.getxyzg();
        Serial.print("X = ");
        Serial.println(xyz[0], 3);
        Serial.print("Y = ");
        Serial.println(xyz[1], 3);
        Serial.print("Z = ");
        Serial.println(xyz[2], 3);
        delay(10);
    }

    delay(5000);
}

// modified github example
/*
#include <Arduino.h>

#include "LSM6DS3.h"
#include "Wire.h"

// Create a instance of class LSM6DS3
LSM6DS3 myIMU; // I2C device address 0x6A

SensorSettings settings;

void setup() {

    Serial.begin(SERIAL_BAUD); // startup serial monitor
    while (!Serial) {
    }

    settings.accelEnabled = 1;
    settings.accelRange;      // check class implimentation for values
    settings.accelSampleRate; // might modify
    settings.accelBandWidth;  // probably dont need it

    // disable gyroscope and temperature sensor
    myIMU.settings.gyroEnabled = 0;
    myIMU.settings.tempEnabled = 0;

    // Call .begin() to configure the IMUs
    if (myIMU.begin() != 0) {
        Serial.println("Device error");
    } else {
        Serial.println("Device OK!");
    }
}

volatile char bit = 0x01;

void loop() {
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X1 = ");
    Serial.println(myIMU.readFloatAccelX(), 4);
    Serial.print(" Y1 = ");
    Serial.println(myIMU.readFloatAccelY(), 4);
    Serial.print(" Z1 = ");
    Serial.println(myIMU.readFloatAccelZ(), 4);

    delay(2000);
    myIMU.settings.accelEnabled ^= 0b1;
    myIMU.begin();
}
    */