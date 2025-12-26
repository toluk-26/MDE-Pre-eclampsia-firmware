#include "bt.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <bluefruit.h>

PES_BLE blemain;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 4000) {
        // wait for serial to connect, for up to 4 seconds (4000 milliseconds)
    }

#ifdef DEBUG
    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("-----------------------------\n");
#endif

    blemain.init();
}

void loop() {
}