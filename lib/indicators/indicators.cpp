#include "indicators.h"

#define LED_RED 2
#define LED_YELLOW 3
#define BUZZER_PIN 4

void Indicators::init() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
}

void Indicators::blinkYellow(int durationMs) {
    unsigned long start = millis();
    while (millis() - start < durationMs) {
        digitalWrite(LED_YELLOW, HIGH);
        delay(500);
        digitalWrite(LED_YELLOW, LOW);
        delay(500);
    }
}

void Indicators::alertHighBP() { digitalWrite(LED_RED, HIGH); }

void Indicators::alertCriticalBP() {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
}