#include <Arduino.h>

#include "rtc.hpp"
#include <inttypes.h>
#include <nrf_rtc.h>

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
    // Serial.print('\n');
    rtc.setAlarm(rtc.getTime() + 60);
}

void loop() {
    if (!rtc.tick()) return;
}