/*
Filename: actigraph.hpp
Author: Samuel Thompson
Description:

Uses accelerometer to measure subject movement
based on a forehead implimentation, and determines if subject is in a
satasfactory position and non-moving state.

All axes defined relative to Seeed Studio Xiao nrf5n840 sense devkit label side
facing subject's face.

Accelerometer vectors described as follows:
+X is pointing up when facing label, and USB-C port is down
+Y is pointing up when facing label, and USB-C port is to the left.
+Z is pointing up off of devkit label.

Macros are used to describe devkit orientation in our device package.
UNCOMMENT ONE MACRO for proper anglular math, based on name describing
orientation.

Adjust class private variables as necessary to fine-tune to application.

Date: 2026/02/09 Modified:
*/

#ifndef ACTIGRAPH_HPP
#define ACTIGRAPH_HPP

#include "accelerometer.hpp"
#include <vector>

// #define usb_c_to_crown
#define usb_c_to_chin
// #define usb_c_to_left_ear
// #define usb_c_to_right_ear
#if (defined(usb_c_to_chin) + defined(usb_c_to_crown) +                        \
     defined(usb_c_to_left_ear) + defined(usb_c_to_right_ear)) != 1
#error "Define exactly one USB-C orientation macro"
#endif

class actigraph {
  public:
    actigraph(int windowLength = 5, int windowCount = 5,
              float movementThreshold = 0.05, int maxHeadAngle = 30);

    // run the accelerometer and collect window data
    bool run();

  private:
    // check if the head angle is acceptable
    bool goodAngle();

    // check if the movement is acceptable
    bool goodMovement();

    // supine head pitch (head forward / back rotation)
    float pitch(float x, float y, float z);

    // supine head yaw (head looking left / right)
    float yaw(float x, float y, float z);

    // mean of a vector of values
    float mean(const std::vector<float> &vals);

    // standard deviation of a vector of values
    float standardDeviation(const std::vector<float> &vals);

    accelerometer xl; // accelerometer object

    std::vector<std::vector<std::vector<float>>>
        data; // 3d vector to store (x,y,z) g's for each window's
              // measurements. data[0] is first window's measurements.
              // data[0][0] is the first xyz measurment: std::vector
              // (x,y,z)

    /** @todo ADJUST FOLLOWING VALUES AS NECESSARY */
    unsigned int windowLength = 5; // number of seconds in a measurement window
    unsigned int windowCount = 5; // number of windows to decide movement status
    unsigned int maxHeadAngle =
        30; // max head angle in degrees to be considered supine

    float movementThreshold = 0.15; // motion standard deviation threshold
};

#endif