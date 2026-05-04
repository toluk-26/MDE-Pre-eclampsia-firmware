/**
 * @file ppg.hpp
 * @author Samuel Thompson,
 * @description: Wrapper class for the MAX30101 sensor to make a BP estimation.
 * @date 2026/05/04
 * @modified:
 */

#ifndef PPG_HPP
#define PPG_HPP

#include <MAX30105.h>

// include another Wire.h object? the actigraph uses one already, maybe create a
// global wire object and pass it to ppg and actigraph

typedef struct bloodPressure {
    float systolic;
    float diastolic;
};

class PPG {
  public:
    // constructor
    // PPG();

    /**
     * @brief
     * @return
     */
    bloodPressure run();

  private:
    // add whatever you need here
};

#endif