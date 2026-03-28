// actigraph class test

#include "actigraph.hpp"
#include <arduino.h>

actigraph ak(3, 3);

void setup() {
    Serial.begin(SERIAL_BAUD); // startup serial monitor
    while (!Serial) {
        ;
    }
}

void loop() {

    Serial.println("\n--RUNNING ACTIGRAPH TEST--");

    bool result = ak.run();
    Serial.print("ACTIGRAPH TEST RESULT: ");
    Serial.println(result);

    delay(10000);
}
