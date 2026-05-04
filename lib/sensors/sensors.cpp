#include "sensors.h"
#include "rtc.hpp"
#include <Arduino.h>

void Sensors::init() {
    // ppg.init(); // run init/setup code for ppg sensor  */
}

/** @todo what is this for? */
void Sensors::calibrate() {
    // delay(2000);

    // for (int i = 0; i < 100; i++) {
    //     ppg.getIR(); /** @todo use actual PPG code */
    //     delay(10);
    // }
}

/**
 * @brief wrapper to run the actigraph and check if movement and position are
 * acceptable for BP measurement
 * @return true if actigraph detects acceptable movement and position, false
 * otherwise
 */
bool Sensors::runActigraph() { return acti.run(); }

/**
 * @brief
 * @return
 */
bool Sensors::rtcTriggered() { return ::rtc.tick(); }

/** @todo implement PPG functions here from using MAX30101.
 * also add reading from config to BP thresholds.
 * */
bloodPressure Sensors::measureBP() {
    // get the systolic and diastolic bp using PPG
    return ppg.run();
}
