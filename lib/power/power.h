#pragma once
#include <Arduino.h>

class Power {
  public:
    bool powerSwitchOn();
    bool isBatteryLow();
    bool isCharging();
    float getBatteryPercent();
    void enterLowPowerMode();
    void scheduleRetry(int minutes);
};