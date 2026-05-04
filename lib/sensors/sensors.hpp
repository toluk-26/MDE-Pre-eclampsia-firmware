/**
 * @file sensors.hpp
 * @authors Lateisha Shahril, Samuel Thompson
 * @description: Wrapper class for all needed sensors: PPG, Accelerometer, RTC
 * @date YYYY/MM/DD
 * @modified:
 */

#ifndef SENSORS_HPP
#define SENSORS_HPP

#include "actigraph.hpp"
#include "ppg.hpp"
#include "rtc.hpp"

class Sensors {
  public:
    /**
     * @brief wrapper to run the actigraph and check if movement and position
     * are acceptable for BP measurement
     * @return true if actigraph detects acceptable movement and position, false
     * otherwise
     */
    bool runActigraph();

    /**
     * @brief
     * @return
     */
    bool rtcTriggered();

    /**
     * @brief wrapper to run the actigraph and check if movement and position
     * are acceptable for BP measurement
     * @return bloodPressure struct holding systolic and diastolic measurements
     */
    bloodPressure measureBP();

  private:
    PPG ppg;
    Actigraph ak;
    RTC rtc;
};

#endif