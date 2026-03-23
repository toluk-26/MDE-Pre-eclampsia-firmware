/*
Filename: ppgTestingArduino.ino
Author: Jisu Lee
Description: This code extracts the raw value (ADC) of infrared light of the PPG
sensor. Separate those values into AC and DC, remove DC component, apply LPF for
HB detection. Measure data for 1 min, generate clean PPG signal using Serial
Plotter, and generate output of PI, min peak, max peak, and mean. The sensor
shuts down for now after measurement.
Date: 2026/02/09 
Modified: 2026/02/23
*/

#include "MAX30105.h"
#include <Wire.h>

MAX30105 particleSensor;

const int sampleRate = 100; // Hz
float irRaw = 0;

#define DC_WINDOW 100

float dcBuffer[DC_WINDOW];
int dcIndex = 0;
float dcSum = 0;
float dcValue = 0;

float filteredAC = 0;
const float alpha = 0.16;

enum State { WARMUP, MEASURING, DONE };
State state = WARMUP;

unsigned long stateStartTime = 0;

float prev = 0;
float prev2 = 0;

float minPeak = 999999;
float maxPeak = -999999;
float meanSum = 0;
long sampleCount = 0;

const unsigned long WARMUP_TIME = 5000;   // 5 seconds
const unsigned long MEASURE_TIME = 60000; // 60 seconds

bool printed = false;

void setup() {
    Serial.begin(115200);

    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30105 not found");
        while (1)
            ;
    }

    byte ledBrightness = 0x1F;
    byte sampleAverage = 4;
    byte ledMode = 2; // Red + IR
    int pulseWidth = 411;
    int adcRange = 4096;

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate,
                         pulseWidth, adcRange);

    stateStartTime = millis();
}

void loop() {
    irRaw = particleSensor.getIR();

    // DC removal
    dcSum -= dcBuffer[dcIndex];
    dcBuffer[dcIndex] = irRaw;
    dcSum += irRaw;

    dcIndex++;
    if (dcIndex >= DC_WINDOW) dcIndex = 0;

    dcValue = dcSum / DC_WINDOW;
    float acValue = irRaw - dcValue;

    // Low pass filter
    filteredAC = filteredAC + alpha * (acValue - filteredAC);

    if (state != DONE) Serial.println(filteredAC);

    unsigned long now = millis();

    // STATE MACHINE
    if (state == WARMUP) {
        if (now - stateStartTime > WARMUP_TIME) {
            state = MEASURING;
            stateStartTime = now;
            Serial.println("#Measurement started");
        }
    }

    else if (state == MEASURING) {
        meanSum += filteredAC;
        sampleCount++;

        // peak detection
        if (prev > prev2 && prev > filteredAC)
            if (prev > maxPeak) maxPeak = prev;

        if (prev < prev2 && prev < filteredAC)
            if (prev < minPeak) minPeak = prev;

        // stop after 60 seconds
        if (now - stateStartTime > MEASURE_TIME) {
            state = DONE;

            float meanValue = meanSum / sampleCount;
            float pulsidx = (maxPeak - minPeak) / abs(meanValue);

            Serial.println("#------ RESULTS ------");
            Serial.print("#Min Peak: ");
            Serial.println(minPeak);
            Serial.print("#Max Peak: ");
            Serial.println(maxPeak);
            Serial.print("#Mean: ");
            Serial.println(meanValue);
            Serial.print("#Pulsatility Index: ");
            Serial.println(pulsidx);
            Serial.println("#---------------------");
        }
    }

    else if (state == DONE) {
        if (!printed) {
            float meanValue = meanSum / sampleCount;
            float pulsidx = (maxPeak - minPeak) / abs(meanValue);

            Serial.println("#------ RESULTS ------");
            Serial.print("#Min Peak: ");
            Serial.println(minPeak);
            Serial.print("#Max Peak: ");
            Serial.println(maxPeak);
            Serial.print("#Mean: ");
            Serial.println(meanValue);
            Serial.print("#Pulsatility Index: ");
            Serial.println(pulsidx);
            Serial.println("#Sensor entering sleep mode");

            particleSensor.shutDown(); // STOPS THE SENSOR

            printed = true;
        }
    }

    prev2 = prev;
    prev = filteredAC;
}