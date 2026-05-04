#include "indicators.h"
#include <Arduino.h>
#define BUTTON_PIN 7
Indicators indic;
bool buttonState = false;
bool ledState = false;

void setup() { indic.init(); }

void loop() {
    // indic.blinkYellow(5000);
    // indic.alertHighBP();
    // indic.alertCriticalBP();
    indic.testRGB(5000);

    // // button test code
    // int button = digitalRead(BUTTON_PIN);

    // if ((button == HIGH) && !buttonState) {
    //     indic.alertHighBP();
    //     buttonState = true;
    //     delay(100);
    // }

    // else if ((button == LOW) && buttonState) {
    //     indic.ledOff();
    //     buttonState = false;
    //     delay(100);
    // }
}