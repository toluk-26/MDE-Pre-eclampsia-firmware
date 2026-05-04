#include "indicators.h"

#define LED_RED 1
#define LED_BLUE 2
#define LED_GREEN 3
#define BUZZER_PIN 0
#define BUTTON_PIN 7

void Indicators::init() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
}

/** @todo this code is blocking. */
void Indicators::blinkYellow(int durationMs) {
    unsigned long start = millis();
    digitalWrite(LED_BLUE, HIGH);
    while (millis() - start < durationMs) {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        delay(250);

        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, HIGH);
        delay(250);
    }
}

void Indicators::testRGB(int durationMs) {
    unsigned long start = millis();
    while (millis() - start < durationMs) {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_BLUE, LOW);
        delay(500);

        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_BLUE, HIGH);
        delay(500);

        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_BLUE, HIGH);
        delay(500);
    }
}

void Indicators::alertHighBP() { digitalWrite(LED_BLUE, LOW); }

void Indicators::ledOff() {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
}

/**
 * @brief  turn on red LED and buzzer for a critical BP alert
 */
void Indicators::alertCriticalBP() {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);

    digitalWrite(LED_RED, LOW);
    tone(BUZZER_PIN, 4000, 500);
    delay(500);
    // digitalWrite(LED_RED, HIGH);
    // tone(BUZZER_PIN, 4000, 500);
    // delay(500);
}

bool Indicators::detectButton() {
    bool buttonState = digitalRead(BUTTON_PIN);
    return buttonState;
}
