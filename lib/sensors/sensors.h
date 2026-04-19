#pragma once
#include "bpstatus.h"
#include "clock.hpp"
#include "actigraph.hpp"
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