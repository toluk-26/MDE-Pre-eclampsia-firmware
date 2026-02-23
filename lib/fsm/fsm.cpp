#include "fsm.h"

FSM::FSM(Sensors &sensors_, Indicators &indicators_, Power &power_)
    : sensors(sensors_), indicators(indicators_), power(power_) {}

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
        // One reading done; power off
        power.enterLowPowerMode();
        break;
    }
}

// ------------------ Handlers ------------------
void FSM::handleInit() {
    sensors.init();
    indicators.init();

    if (power.isBatteryLow())
        currentState = STATE_LOW_BATT;
    else
        currentState = STATE_CALIBRATE;
}

void FSM::handleLowBattery() {
    indicators.blinkYellow(10000); // 10 seconds
    if (power.isCharging()) currentState = STATE_CHARGING;
}

void FSM::handleCharging() {
    indicators.blinkYellow(500); // fast blink
    if (!power.isCharging() && power.getBatteryPercent() > 30)
        currentState = STATE_INIT;
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
        power.scheduleRetry(20); // wait 20 min
        currentState = STATE_IDLE;
    }
}

void FSM::handleMeasureBP() {
    BPStatus status = sensors.measureBP();
    if (status == BP_CRITICAL)
        currentState = STATE_BP_CRITICAL;
    else if (status == BP_HIGH)
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