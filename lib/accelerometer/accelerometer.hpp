/*
Filename: accelerometer.hpp
Author: Samuel Thompson
Description:        add desc
Date: 2026/02/07
Modified:
*/

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "LSM6DS3.h"
#include <vector>

class accelerometer {
  public:
    // constructor
    accelerometer();

    // turn the accelerometer on
    void start();

    // turn the accelerometer off
    void stop();

    // Get a vector of (x,y,z) g's.
    // If performing data analysis, recommend waiting at least 10ms between
    // reads to avoid duplicate force values; 10ms due to the 104hz ODR.
    std::vector<float> getxyzg();

  private:
    // get g's on x vector
    float getxg();

    // get g's on y vector
    float getyg();

    // get g's on z vector
    float getzg();

    LSM6DS3 IMU;
};

#endif