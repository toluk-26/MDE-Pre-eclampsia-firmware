#include "power.h"
#include "clock.hpp"
#include "clock.cpp"

#define BATTERY_PIN A0
#define CHARGER_PIN 5

bool Power::powerSwitchOn() { return true; }      // implement switch detection
bool Power::isBatteryLow() { return false; }      // read battery
bool Power::isCharging() { return false; }        // detect charger
float Power::getBatteryPercent() { return 50.0; } // map ADC voltage

void Power::enterLowPowerMode() {
    // implement deep sleep, mode must specifically allow the rtc to conitnue the function
}

void Power::scheduleRetry(int minutes) {
    uint64_t currentTime = clock.getTime();
    uint64_t wakeTime = currentTime + (minutes * 60);

    clock.setAlarm(wakeTime);

#ifdef DEBUG
    Serial.print("Retry scheduled in ");
    Serial.print(minutes);
    Serial.println(" minutes");
#endif
}
