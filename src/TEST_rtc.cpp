#include <Arduino.h>

#include "clock.hpp"

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < SERIAL_WAIT_TIME) {
        // wait for serial to connect, for up to 10 seconds (10000 milliseconds)
    }
    delay(500);

    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("        RTC Program");
    Serial.println("-----------------------------\n");
}

void loop() {}