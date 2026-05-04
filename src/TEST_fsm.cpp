#include <Arduino.h>

#include "bt.hpp"
#include "fsm.h"
#include "indicators.h"
#include "power.h"
#include "sensors.h"

Sensors sensors;
Indicators indicators;
Power power;
PESBt bt;

FSM fsm(sensors, indicators, power, bt);

void setup() {
#ifdef DEBUG
    Serial.begin(115200); 
    delay(1000);
    Serial.println("=== FSM TEST ===");
#endif
}

void loop() {
#ifdef DEBUG
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        Serial.println("FSM running...");
        lastPrint = millis();
    }
#endif

    fsm.run();
}