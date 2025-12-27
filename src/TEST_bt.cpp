
#include "bt.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <bluefruit.h>

PESBt bt;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < SERIAL_WAIT_TIME) {
        // wait for serial to connect, for up to 10 seconds (10000 milliseconds)
    }

#ifdef DEBUG
    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("-----------------------------\n");
#endif

    bt.init();
}

void loop() {}