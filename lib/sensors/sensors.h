#pragma once
#include "actigraph.hpp"
#include "bpstatus.h"
#include "rtc.hpp"
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
    actigraph acti;
};