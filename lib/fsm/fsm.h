#pragma once
#include "indicators.h"
#include "power.h"
#include "sensors.h"

class FSM {
  public:
    FSM(Sensors &sensors, Indicators &indicators, Power &power);
    void run();

  private:
    enum SystemState {
        STATE_OFF,
        STATE_INIT,
        STATE_LOW_BATT,
        STATE_CHARGING,
        STATE_CALIBRATE,
        STATE_IDLE,
        STATE_CONDITION_CHECK,
        STATE_MEASURE_BP,
        STATE_BP_HIGH,
        STATE_BP_CRITICAL,
        STATE_DONE
    };

    SystemState currentState = STATE_OFF;

    // BP result enum
    enum BPStatus { BP_OK, BP_HIGH, BP_CRITICAL };

    // References to modular components
    Sensors &sensors;
    Indicators &indicators;
    Power &power;

    // FSM helpers
    void handleInit();
    void handleLowBattery();
    void handleCharging();
    void handleCalibrate();
    void handleIdle();
    void handleConditionCheck();
    void handleMeasureBP();
    void handleBPHigh();
    void handleBPCritical();
};