#pragma once
#include <Arduino.h>

class Indicators {
  public:
    void init();
    void blinkYellow(int durationMs);
    void testRGB(int durationMs);
    void alertHighBP();
    void alertCriticalBP();
    bool detectButton();
    void ledOff();
};