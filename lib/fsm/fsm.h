#pragma once
#include "bpstatus.h" // include BPStatus enum
#include "bt.hpp"
#include "indicators.h"
#include "power.h"
#include "sensors.h"

class FSM {
  public:
    FSM(Sensors &sensors, Indicators &indicators, Power &power, PESBt &bt);
    void run();

  private:
    enum SystemState {
        STATE_OFF,
        STATE_INIT,
        STATE_LOW_BATT,
        STATE_CHARGING,
        STATE_CALIBRATE,
        STATE_CHECK_CONFIG,
        STATE_IDLE,
        STATE_CONDITION_CHECK,
        STATE_MEASURE_BP,
        STATE_BP_HIGH,
        STATE_BP_CRITICAL,
        STATE_DONE
    };

    SystemState currentState = STATE_OFF;

    Sensors &sensors;
    Indicators &indicators;
    Power &power;
    PESBt &bt;

    void handleInit();
    void handleLowBattery();
    void handleCharging();
    void handleCheckConfig();
    void handleCalibrate();
    void handleIdle();
    void handleConditionCheck();
    void handleMeasureBP();
    void handleBPHigh();
    void handleBPCritical();
};