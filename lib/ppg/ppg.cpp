/**
 * @file ppg.cpp
 * @author Samuel Thompson, Jisu Lee
 * @description: PPG class implementation.
 * @date 2026/05/04
 * @modified: 2026/05/04 
 */

#include "ppg.hpp"
#include "log.hpp"
#include <Arduino.h>
#include <Wire.h>
#include <math.h>

namespace {
const int SAMPLE_RATE_HZ = 100;
const unsigned long SAMPLE_INTERVAL_MS = 1000UL / SAMPLE_RATE_HZ;

const float FILTER_ALPHA = 0.16f;
const float MIN_MEAN_MAGNITUDE = 0.0001f;

const unsigned long WARMUP_TIME_MS = 5000UL;
const unsigned long MEASURE_TIME_MS = 60000UL;

const byte LED_BRIGHTNESS = 0x1F;
const byte SAMPLE_AVERAGE = 4;
const byte LED_MODE = 2; // this is red and IR. this was the best choice for us, but theoretically IR gives good results for different skin tones.
const int PULSE_WIDTH = 411;
const int ADC_RANGE = 4096;

/*
below are the linear regression coefficients from MATLAB
MATLAB trains the model using measured PI values and cuff BP values.
this system only applies the final model

SBP = SYSTOLIC_COEFF_M * PI + SYSTOLIC_COEFF_B
DBP = DIASTOLIC_COEFF_M * PI + DIASTOLIC_COEFF_B

these values should be updated whenever the MATLAB regression dataset changes.
*/
const float SYSTOLIC_COEFF_M = 0.20962f;
const float SYSTOLIC_COEFF_B = 110.42319f;
const float DIASTOLIC_COEFF_M = -0.10466f;
const float DIASTOLIC_COEFF_B = 80.48624f;
} // namespace

bloodPressure PPG::run() {
    bloodPressure results = {0.0f, 0.0f};

    resetProcessingState();
    if (!beginSensor()) {
        LOGV("Sensor not found");
        return results;
    }

    unsigned long lastSampleTime = 0;
    unsigned long stateStartTime = millis();

    while ((millis() - stateStartTime) < WARMUP_TIME_MS) {
        waitForNextSample(lastSampleTime);
        float filteredSample = processSample((float)particleSensor.getIR());
#ifdef DEBUG_PPG_PLOT
        Serial.println(filteredSample);
#endif
        updatePeakHistory(filteredSample);
    }

    LOGV("PPG measurement started");
    stateStartTime = millis();

    while ((millis() - stateStartTime) < MEASURE_TIME_MS) {
        waitForNextSample(lastSampleTime);
        float filteredSample = processSample((float)particleSensor.getIR());
#ifdef DEBUG_PPG_PLOT
        Serial.println(filteredSample);
#endif
        updateMeasurementStats(filteredSample);
        updatePeakHistory(filteredSample);
    }

    lastPulsatilityIndex = calculatePulsatilityIndex();
    results = estimateBloodPressure(lastPulsatilityIndex);

    LOGV("PPG Min Peak: %f", lastMinPeak);
    LOGV("PPG Max Peak: %f", lastMaxPeak);
    LOGV("PPG Mean: %f", lastMean);
    LOGV("PPG Pulsatility Index: %f", lastPulsatilityIndex);

    particleSensor.shutDown();

    return results;
}

float PPG::getLastPulsatilityIndex() const { return lastPulsatilityIndex; }

float PPG::getLastMinPeak() const { return lastMinPeak; }

float PPG::getLastMaxPeak() const { return lastMaxPeak; }

float PPG::getLastMean() const { return lastMean; }

bool PPG::beginSensor() {
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        return false;
    }

    particleSensor.wakeUp();
    configureSensor();
    particleSensor.clearFIFO();
    return true;
}

void PPG::configureSensor() {
    particleSensor.setup(LED_BRIGHTNESS, SAMPLE_AVERAGE, LED_MODE,
                         SAMPLE_RATE_HZ, PULSE_WIDTH, ADC_RANGE);
}

void PPG::resetProcessingState() {
    for (size_t i = 0; i < DC_WINDOW; i++) {
        dcBuffer[i] = 0.0f;
    }

    dcIndex = 0;
    dcSum = 0.0f;
    filteredAC = 0.0f;

    prev = 0.0f;
    prev2 = 0.0f;

    minPeak = 0.0f;
    maxPeak = 0.0f;
    meanSum = 0.0f;
    sampleCount = 0;
    hasMinPeak = false;
    hasMaxPeak = false;

    lastMinPeak = 0.0f;
    lastMaxPeak = 0.0f;
    lastMean = 0.0f;
    lastPulsatilityIndex = 0.0f;
}

void PPG::waitForNextSample(unsigned long &lastSampleTime) const {
    unsigned long now = millis();
    if (lastSampleTime == 0) {
        lastSampleTime = now;
        return;
    }

    unsigned long elapsed = now - lastSampleTime;
    if (elapsed < SAMPLE_INTERVAL_MS) {
        delay(SAMPLE_INTERVAL_MS - elapsed);
    }

    lastSampleTime = millis();
}

float PPG::processSample(float irRaw) {
    dcSum -= dcBuffer[dcIndex];
    dcBuffer[dcIndex] = irRaw;
    dcSum += irRaw;

    dcIndex++;
    if (dcIndex >= DC_WINDOW) {
        dcIndex = 0;
    }

    float dcValue = dcSum / (float)DC_WINDOW;
    float acValue = irRaw - dcValue;
    filteredAC = filteredAC + FILTER_ALPHA * (acValue - filteredAC);

    return filteredAC;
}

void PPG::updatePeakHistory(float filteredSample) {
    prev2 = prev;
    prev = filteredSample;
}

void PPG::updateMeasurementStats(float filteredSample) {
    meanSum += filteredSample;
    sampleCount++;

    if (prev > prev2 && prev > filteredSample) {
        if (!hasMaxPeak || prev > maxPeak) {
            maxPeak = prev;
            hasMaxPeak = true;
        }
    }

    if (prev < prev2 && prev < filteredSample) {
        if (!hasMinPeak || prev < minPeak) {
            minPeak = prev;
            hasMinPeak = true;
        }
    }
}

float PPG::calculatePulsatilityIndex() {
    if (sampleCount == 0) {
        return 0.0f;
    }

    lastMean = meanSum / (float)sampleCount;

    if (!hasMinPeak || !hasMaxPeak || fabs(lastMean) < MIN_MEAN_MAGNITUDE) {
        lastMinPeak = minPeak;
        lastMaxPeak = maxPeak;
        return 0.0f;
    }

    lastMinPeak = minPeak;
    lastMaxPeak = maxPeak;
    // PI = max peak - min peak / |mean|. this is passed into the regression model to estimate SBP and DBP.
    return (lastMaxPeak - lastMinPeak) / fabs(lastMean);
}

bloodPressure PPG::estimateBloodPressure(float pulsatilityIndex) const {
    bloodPressure results;
    results.systolic =
        SYSTOLIC_COEFF_M * pulsatilityIndex + SYSTOLIC_COEFF_B;
    results.diastolic =
        DIASTOLIC_COEFF_M * pulsatilityIndex + DIASTOLIC_COEFF_B;
    return results;
}
