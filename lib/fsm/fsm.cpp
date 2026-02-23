#include "fsm.h"

enum SystemState {
    STATE_OFF,
    STATE_INIT,
    STATE_LOW_BATT,
    STATE_CHARGING,
    STATE_IDLE,
    STATE_CONDITION_CHECK,
    STATE_MEASURE_BP,
    STATE_BP_HIGH,
    STATE_BP_CRITICAL,
    STATE_DONE
};

static SystemState currentState = STATE_OFF;

void initFSM() {
    currentState = STATE_INIT;
}

void runFSM() {
    switch (currentState) {

    case STATE_OFF:
        if (powerSwitchOn())
            currentState = STATE_INIT;
        break;

    case STATE_INIT:
        initSensors();
        initIndicators();
        initRTC();

        if (isBatteryLow())
            currentState = STATE_LOW_BATT;
        else
            currentState = STATE_IDLE;
        break;

    case STATE_LOW_BATT:
        blinkYellow();
        if (isCharging())
            currentState = STATE_CHARGING;
        break;

    case STATE_CHARGING:
        blinkYellow();
        if (!isCharging() && getBatteryPercent() > 30)
            currentState = STATE_INIT;
        break;

    case STATE_IDLE:
        enterLowPowerMode();
        if (rtcTriggered())
            currentState = STATE_CONDITION_CHECK;
        break;

    case STATE_CONDITION_CHECK:
        if (isUserStill() && isPositionValid())
            currentState = STATE_MEASURE_BP;
        else {
            scheduleRetry(20);
            currentState = STATE_IDLE;
        }
        break;

    case STATE_MEASURE_BP:
        BPStatus status = measureBP();

        if (status == BP_CRITICAL)
            currentState = STATE_BP_CRITICAL;
        else if (status == BP_HIGH)
            currentState = STATE_BP_HIGH;
        else
            currentState = STATE_DONE;
        break;

    case STATE_BP_HIGH:
        blinkRed();
        currentState = STATE_DONE;
        break;

    case STATE_BP_CRITICAL:
        blinkRed();
        soundBuzzer();
        currentState = STATE_DONE;
        break;
    }
}