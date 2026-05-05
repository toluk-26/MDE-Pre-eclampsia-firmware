#include "new_fsm.hpp"

// contains power detection, for idle to charging state transitions
#include "battery.h"

// FSM::FSM(Sensors &sensors, Indicators &indicators, Power &power, BleManager
// &bt,
//          Log &log, FlashLog &flash)
//     : sensors(sensors), indicators(indicators), power(power), bt(bt),
//     log(log),
//       flash(flash) {}

void FSM::run() {
    rtc.tick();
    switch (currentState) {
    case STATE_INIT:
        handleInit();
        break;
    case STATE_IDLE:
        handleIdle();
        break;
    case STATE_CHECK_ACTIGRAPH:
        handleCheckActigraph();
        break;
    case STATE_MEASURE_BP:
        handleMeasureBP();
        break;
    case STATE_ALERT_WARNING:
        handleAlertWarning();
        break;
    case STATE_ALERT_CRITICAL:
        handleAlertCritical();
        break;
    case STATE_CHARGING:
        handleCharging();
        break;
    }
}

void FSM::handleInit() {
    // enable power detection
    power_init();
    // intialize QSPI flash memory
    flash.begin();
    // initialize user indicators
    indicators.init();

    /** @todo ensure no other initialization is needed*/

    // spin in init until charging
    while (power_get_state() == POWER_BATTERY) {
        ;
    }
    currentState = STATE_CHARGING;
}

void FSM::handleIdle() {
    /** @todo enter low power mode and wake on RTC interrupt*/
    // power.enterLowPowerMode();

    /** @todo ensure that rtc alarm is what causes transition */
    if (sensors.rtcTriggered()) {
        currentState = STATE_CHECK_ACTIGRAPH;

    }
    // if power through the usb is detected, move to charging state.
    else if (power_get_state() == POWER_USB) {
        currentState = STATE_CHARGING;
    }
}

void FSM::handleCheckActigraph() {
    // run the actigraph
    if (sensors.runActigraph()) {
        currentState = STATE_MEASURE_BP;
    }
    // reschedule measurements with bad movement / positioning
    else {
        power.scheduleRetry(30);
        currentState = STATE_IDLE;
    }
}

void FSM::handleMeasureBP() {
    // runs the PPG sensor
    bloodPressure results = sensors.measureBP();
    uint8_t diastolic = uint8_t(results.diastolic);
    uint8_t systolic = uint8_t(results.systolic);

    /** @todo what does 'code' do? edit HR later */
    uint8_t defaultHR = 90;
    uint8_t code = 0;

    // logs the PPG reading
    log.sensor(defaultHR, diastolic, systolic, code);

    // check if BP conditions are violated
    ConfigPack config = flash.getConfig();

    // check warning BP levels first
    if (diastolic > config.diastolic_max || systolic > config.systolic_max ||
        diastolic < config.diastolic_min || systolic < config.systolic_min) {
        currentState = STATE_ALERT_WARNING;

        /** @todo check for extreme / critical levels next*/
        // if (diastolic > config.diastolic_max_crit ||
        //     systolic > config.systolic_max_crit ||
        //     diastolic < config.diastolic_min_crit ||
        //     systolic < config.systolic_min_crit) {
        //     currentState = STATE_ALERT_CRITICAL;
        // }
    }
    // otherwise, return to IDLE state
    else {
        currentState = STATE_IDLE;
    }
}

void FSM::handleAlertWarning() {
    // activate high BP alert (red led), then go back to idle
    indicators.alertHighBP();
    currentState = STATE_IDLE;
}

void FSM::handleAlertCritical() {
    /** @todo handle critical alert */
    // should more or less follow the warning alert code
}

void FSM::handleCharging() {
    // move to idle on battery power
    if (power_get_state() == POWER_BATTERY && !bt.isConnected()) {
        currentState = STATE_IDLE;
        bt.stop(); // turn off BT
        runOnce = true;
    }

    // start bluetooth
    if (runOnce) {
        bt.begin();
        runOnce = false;
    }
    if (bt.transferService.transfer_flag && tcrtl.isDone()) {
        tcrtl.begin();
    }

    if (!tcrtl.isDone()) {
        tcrtl.run();
    }

    // TODO: calibrate. send PI. change to float?
    if (bt.calibrateService.stream_flag) sctrl.run(0);

    if (bt.disconnectFlag) {
        bt.configService.saveConfig();
        bt.disconnectFlag = false;
    }
}