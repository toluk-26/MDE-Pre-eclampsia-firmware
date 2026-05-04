// #pragma once
// #include "bt.hpp"
// #include "indicators.h"
// #include "power.h"
// #include "sensors.h"

// class FSM {
//   public:
//     /**
//      * @brief FSM class constructor.
//      */
//     FSM(Sensors &sensors, Indicators &indicators, Power &power, BleManager
//     &bt);

//     /**
//      * @brief primary method to run the FSM, should be called in the main
//      loop
//      * of the program.
//      */
//     void run();

//   private:
//     enum SystemState {
//         STATE_OFF,
//         STATE_INIT,
//         STATE_LOW_BATT,
//         STATE_CHARGING,
//         STATE_CALIBRATE,
//         STATE_CHECK_CONFIG,
//         STATE_IDLE,
//         STATE_CONDITION_CHECK,
//         STATE_MEASURE_BP,
//         STATE_BP_HIGH,
//         STATE_BP_CRITICAL,
//         STATE_DONE
//     };

//     SystemState currentState = STATE_OFF;
//     BleManager &bt;
//     Sensors &sensors;
//     Indicators &indicators;
//     Power &power;

//     void handleInit();
//     void handleLowBattery();
//     void handleCharging();
//     void handleCheckConfig();
//     void handleCalibrate();
//     void handleIdle();
//     void handleConditionCheck();
//     void handleMeasureBP();
//     void handleBPHigh();
//     void handleBPCritical();
// };