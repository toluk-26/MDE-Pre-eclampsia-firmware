#include "bt.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <bluefruit.h>

PES_BLE blemain;

void setup() {
    Serial.begin(115200);
    blemain.init();
}

void loop() {
    // Forward data from HW Serial to BLEUART
    while (Serial.available()) {
        // Delay to wait for enough input, since we have a limited transmission
        // buffer
        delay(2);

        uint8_t buf[64];
        int count = Serial.readBytes(buf, sizeof(buf));
        if (buf[0] == '0') {
            blemain.disconnect();
        }
        bleuart.write(buf, count);
    }

    // Forward from BLEUART to HW Serial
    while (bleuart.available()) {
        uint8_t ch;
        ch = (uint8_t)bleuart.read();
        Serial.write(ch);
    }
}