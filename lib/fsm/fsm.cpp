// #include "fsm.h"
// #include "flashlog.hpp"

// FSM::FSM(Sensors &sensors, Indicators &indicators, Power &power, BleManager
// &bt)
//     : sensors(sensors), indicators(indicators), power(power), bt(bt) {}

// void FSM::run() {
//     switch (currentState) {
//     case STATE_OFF:
//         if (power.powerSwitchOn()) currentState = STATE_INIT;
//         break;
//     case STATE_INIT:
//         handleInit();
//         break;
//     case STATE_LOW_BATT:
//         handleLowBattery();
//         break;
//     case STATE_CHECK_CONFIG:
//         handleCheckConfig();
//         break;
//     case STATE_CHARGING:
//         handleCharging();
//         break;
//     case STATE_CALIBRATE:
//         handleCalibrate();
//         break;
//     case STATE_IDLE:
//         handleIdle();
//         break;
//     case STATE_CONDITION_CHECK:
//         handleConditionCheck();
//         break;
//     case STATE_MEASURE_BP:
//         handleMeasureBP();
//         break;
//     case STATE_BP_HIGH:
//         handleBPHigh();
//         break;
//     case STATE_BP_CRITICAL:
//         handleBPCritical();
//         break;
//     case STATE_DONE:
//         power.enterLowPowerMode();
//         break;
//     }
// }

// void FSM::handleInit() {
//     // sensors.init();
//     indicators.init();
//     if (power.isBatteryLow())
//         currentState = STATE_LOW_BATT;
//     else
//         currentState = STATE_CHECK_CONFIG;
// }

// void FSM::handleLowBattery() {
//     indicators.blinkYellow(10000);
//     if (power.isCharging()) currentState = STATE_CHARGING;
// }

// void FSM::handleCharging() {
//     indicators.blinkYellow(500);
//     if (!power.isCharging() &&
//         power.getBatteryPercent() > 30) // low battery is below 30 percent
//         currentState = STATE_INIT;
// }

// /**
//  * @todo what is this for?
//  */
// void FSM::handleCheckConfig() {
//     // // Attempt to load config from flash
//     // bool configValid = mem.getConfig(); // true if config loaded correctly

//     // if (!configValid) {
//     //     LOGE("invalid or missing config. Resetting defaults...");
//     //     mem.cleanConfig(); // erase config sector
//     //     mem.setConfig();   // write default config using FlashLog's
//     internal
//     //                        // defaults
//     //     LOGV3("Default config written");
//     // } else {
//     //     LOGV3("CONFIG: Valid config found");
//     // }
//     // currentState = STATE_CALIBRATE;
// }

// /** @todo unsure what this does */
// void FSM::handleCalibrate() {
//     // sensors.calibrate();
//     currentState = STATE_IDLE;
// }

// /** @todo this should use an interrupt from the rtc to wake up.
//  * this should also enter config mode if the device is connected to power.
//  */
// void FSM::handleIdle() {
//     power.enterLowPowerMode();
//     if (sensors.rtcTriggered()) currentState = STATE_CONDITION_CHECK;
// }

// /**
//  * @brief checks the patients movement and head position using the actigraph.
//  if
//  * the result is acceptable, the fsm moves to measure BP, otherwise it
//  schedules
//  * a retry and goes back to idle.
//  */
// void FSM::handleConditionCheck() {
//     if (sensors.runActigraph()) {
//         currentState = STATE_MEASURE_BP;
//     } else {
//         power.scheduleRetry(60); // changed from 20 mins to 60 mins as per
//         Sam's
//                                  // comment on REM sleep
//         currentState = STATE_IDLE;
//     }
// }

// /**
//  * @brief uses the PPG sensor to measure the paitent's blood pressure and
//  moves
//  * to the appropriate state based on the result
//  */
// void FSM::handleMeasureBP() {
//     // /** @todo this should read the bp log and not just alert off of
//     immediate
//     //  * BP measurement */
//     // uint8_t status = sensors.measureBP();
//     // if (status == BP_CRITICAL)
//     //     currentState = STATE_BP_CRITICAL;
//     // else if (status == BP_ELEVATED)
//     //     currentState = STATE_BP_HIGH;
//     // else
//     //     currentState = STATE_DONE;
// }

// void FSM::handleBPHigh() {
//     indicators.alertHighBP();
//     currentState = STATE_DONE;
// }

// void FSM::handleBPCritical() {
//     indicators.alertCriticalBP();
//     currentState = STATE_DONE;
// }
