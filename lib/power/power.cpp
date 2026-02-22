#include "power.h"

#define BATTERY_PIN A0
#define CHARGER_PIN 5

bool Power::powerSwitchOn() { return true; }      // implement switch detection
bool Power::isBatteryLow() { return false; }      // read battery
bool Power::isCharging() { return false; }        // detect charger
float Power::getBatteryPercent() { return 50.0; } // map ADC voltage

void Power::enterLowPowerMode() {
    // implement deep sleep
}

void Power::scheduleRetry(int minutes) {
    // implement timer
}