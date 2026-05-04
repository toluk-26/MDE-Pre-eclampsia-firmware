/**
 * @file ppg.hpp
 * @author Samuel Thompson, Jisu Lee
 * @description: Wrapper class for the MAX30101 sensor to make a BP estimation
 * (using a pre-trained regression model).
 * @date 2026/05/04
 * @modified: 2026/05/04
 */

#pragma once

#include <MAX30105.h>

// include another Wire.h object? the actigraph uses one already, maybe create a
// global wire object and pass it to ppg and actigraph
/** @brief Stores estimated blood pressure values in mmHg.
 *
 */
typedef struct bloodPressure {
    float systolic;
    float diastolic;
} bloodPressure;

class PPG {
  public:
    // constructor
    // PPG();

    /**
     * @brief runs one blocking PPG measurement. The sensor collects PPG data
     * for one min Extracts: min peak, max peak, mean value, and pulsatility
     * index. Estimates blood pressure using linear regression coefficients
     * obtained from MATLAB.
     * @return bloodPressure struct holding estimated SBP and DBP.
     */
    bloodPressure run();

    float getLastPulsatilityIndex() const;
    float getLastMinPeak() const;
    float getLastMaxPeak() const;
    float getLastMean() const;

  private:
    static const size_t DC_WINDOW = 100;

    bool beginSensor();
    void configureSensor();
    void resetProcessingState();
    void waitForNextSample(unsigned long &lastSampleTime) const;
    float processSample(float irRaw);
    void updatePeakHistory(float filteredSample);
    void updateMeasurementStats(float filteredSample);
    float calculatePulsatilityIndex();

    /** @brief Applies the MATLAB-trained linear regression model.
     * this does not train a model on the microcontroller. it only uses fixed
     * slope/intercept values found in matlab.
     */
    bloodPressure estimateBloodPressure(float pulsatilityIndex) const;

    MAX30105 particleSensor;

    float dcBuffer[DC_WINDOW] = {};
    size_t dcIndex = 0;
    float dcSum = 0.0f;
    float filteredAC = 0.0f;

    float prev = 0.0f;
    float prev2 = 0.0f;

    float minPeak = 0.0f;
    float maxPeak = 0.0f;
    float meanSum = 0.0f;
    unsigned long sampleCount = 0;
    bool hasMinPeak = false;
    bool hasMaxPeak = false;

    float lastMinPeak = 0.0f;
    float lastMaxPeak = 0.0f;
    float lastMean = 0.0f;
    float lastPulsatilityIndex = 0.0f;
};