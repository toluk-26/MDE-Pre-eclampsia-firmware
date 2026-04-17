#include "fsm.h"
#include "flashlog.hpp" 

FSM::FSM(Sensors &sensors_, Indicators &indicators_, Power &power_, PESBt &bt_)
    : sensors(sensors_), indicators(indicators_), power(power_), bt(bt_) {}

void FSM::run() {
    switch (currentState) {
    case STATE_OFF:
        if (power.powerSwitchOn()) currentState = STATE_INIT;
        break;
    case STATE_INIT:
        handleInit();
        break;
    case STATE_LOW_BATT:
        handleLowBattery();
        break;
    case STATE_CHECK_CONFIG:
        handleCheckConfig();
        break;
    case STATE_CHARGING:
        handleCharging();
        break;
    case STATE_CALIBRATE:
        handleCalibrate();
        break;
    case STATE_IDLE:
        handleIdle();
        break;
    case STATE_CONDITION_CHECK:
        handleConditionCheck();
        break;
    case STATE_MEASURE_BP:
        handleMeasureBP();
        break;
    case STATE_BP_HIGH:
        handleBPHigh();
        break;
    case STATE_BP_CRITICAL:
        handleBPCritical();
        break;
    case STATE_DONE:
        power.enterLowPowerMode();
        break;
    }
}

void FSM::handleInit() {
    sensors.init();
    indicators.init();
    bt.init();
    if (!bt.isConnected()) {
        currentState = STATE_INIT; // stay in INIT
        return;
    }
    if (power.isBatteryLow())
        currentState = STATE_LOW_BATT;
    else
        currentState = STATE_CHECK_CONFIG;
}

void FSM::handleLowBattery() {
    indicators.blinkYellow(10000);
    if (power.isCharging()) currentState = STATE_CHARGING;
}

void FSM::handleCharging() {
    indicators.blinkYellow(500);
    if (!power.isCharging() && power.getBatteryPercent() > 30) //low battery is below 30 percent
        currentState = STATE_INIT;
}

void FSM::handleCheckConfig() {
    // Attempt to load config from flash
    bool configValid = mem.getConfig(); // true if config loaded correctly

    if (!configValid) {
#ifdef DEBUG
        Serial.println(
            "CONFIG ERROR: invalid or missing config. Resetting defaults...");
#endif
        mem.cleanConfig(); // erase config sector
        mem.setConfig();   // write default config using FlashLog's internal
                           // defaults
#ifdef DEBUG
        Serial.println("CONFIG: Default config written");
#endif
    } else {
#ifdef DEBUG
        Serial.println("CONFIG: Valid config found");
#endif
    }
    currentState = STATE_CALIBRATE;
}

void FSM::handleCalibrate() {
    sensors.calibrate();
    currentState = STATE_IDLE;
}

void FSM::handleIdle() {
    power.enterLowPowerMode();
    if (sensors.rtcTriggered()) currentState = STATE_CONDITION_CHECK;
}

void FSM::handleConditionCheck() {
    if (sensors.motionOK() && sensors.positionOK())
        currentState = STATE_MEASURE_BP;
    else {
        power.scheduleRetry(60); //changed from 20 mins to 60 mins as per Sam's comment on REM sleep
        currentState = STATE_IDLE;
    }
}

void FSM::handleMeasureBP() {
    BPStatus status = sensors.measureBP();
    if (status == BPStatus::CRITICAL)
        currentState = STATE_BP_CRITICAL;
    else if (status == BPStatus::ELEVATED)
        currentState = STATE_BP_HIGH;
    else
        currentState = STATE_DONE;
}

void FSM::handleBPHigh() {
    indicators.alertHighBP();
    currentState = STATE_DONE;
}

void FSM::handleBPCritical() {
    indicators.alertCriticalBP();
    currentState = STATE_DONE;
}
