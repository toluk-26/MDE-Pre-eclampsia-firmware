#pragma once
#include "bpstatus.h"
#include "clock.hpp"
#include <LSM6DS3.h>
#include <MAX30105.h>

class Sensors {
  public:
    void init();
    void calibrate();
    bool motionOK();
    bool positionOK();
    bool rtcTriggered();

    BPStatus measureBP(); // uses independent enum

  private:
    MAX30105 ppg;
    LSM6DS3 imu{I2C_MODE, 0x6A};
};