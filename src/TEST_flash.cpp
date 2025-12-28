#include <Arduino.h>

#include "flashlog.hpp"

#if !(DEBUG || DEBUG_FLASH)
#error "you seem to be in the wrong env"
#endif

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < SERIAL_WAIT_TIME) {
        // wait for serial to connect, for up to 10 seconds (10000 milliseconds)
    }

    Serial.println("-----------------------------");
    Serial.printf("PreEclampsia Screener v%s\n", SOFTWARE_REVISION);
    Serial.println("        Flash Program");
    Serial.println("-----------------------------\n");

    mem.cleanAll();
    LOG_SENSOR(91, 90, 120, 0);
    LOG_SENSOR(92, 92, 123, 0);
    LOG_SENSOR(93, 90, 120, 0);
    LOG_SENSOR(94, 92, 123, 0);

    mem.printInfo();
    mem.printConfig();
    mem._findTail();
    mem.printMeta();
    mem.printData();

    mem.dumpConfig();
    mem.dumpData();


}

void loop() {}
