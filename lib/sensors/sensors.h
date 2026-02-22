#pragma once
#include <LSM6DS3.h>
#include <MAX30105.h>
#include <RTCZero.h>

class Sensors {
  public:
    void init();
    void calibrate();
    bool motionOK();
    bool positionOK();
    bool rtcTriggered();
    FSM::BPStatus measureBP();

  private:
    MAX30105 ppg;
    LSM6DS3 imu{I2C_MODE, 0x6A};
    RTCZero rtc;
};