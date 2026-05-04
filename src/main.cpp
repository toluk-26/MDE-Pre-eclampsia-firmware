#include "bt.hpp"
#include "indicators.h"
#include "log.hpp"
#include "new_fsm.hpp"
#include "power.h"
#include "sensors.hpp"
#include <Arduino.h>

Sensors sensors;
Indicators indicators;
Power power;
BleManager bluetooth;
Log logger;
FlashLog flash;

FSM program(sensors, indicators, power, bluetooth, logger, flash);

void setup() {
    /*Serial.begin(115200);
    // wait for serial to connect, for up to 10 seconds (10000 milliseconds)
    while (!Serial && millis() < SERIAL_WAIT_TIME) {
        ;
    }*/

#ifdef DEBUG
    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("        Main Program");
    Serial.println("-----------------------------\n");
#endif
}

void loop() { program.run(); }