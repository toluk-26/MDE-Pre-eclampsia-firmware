#pragma once
#include "actigraph.hpp"
#include "ppg.h"

#define BP_OK 0
#define BP_ELEVATED 1
#define BP_CRITICAL 2

class Sensors {
  public:
    void init();
    void calibrate();
    bool runActigraph();
    bool rtcTriggered();

    bloodPressure measureBP();

  private:
    /** @todo add support for PPG */
    PPG ppg;
    actigraph acti; // auto-constructs actigraph object
};