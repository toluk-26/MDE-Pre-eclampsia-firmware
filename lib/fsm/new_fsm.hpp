#ifndef NEW_FSM_HPP
#define NEW_FSM_HPP

#include "bt.hpp"
#include "flashlog.hpp"
#include "indicators.h"
#include "log.hpp"
#include "power.h"
#include "sensors.hpp"

class FSM {
  public:
    /**
     * @brief FSM class constructor.
     */
    FSM(Sensors &sensors, Indicators &indicators, Power &power, BleManager &bt,
        Log &log, FlashLog &flash);

    /**
     * @brief primary method to run the FSM, should be called in the main loop
     * of the program.
     */
    void run();

  private:
    enum SystemState {
        /**
         * @brief device program startup initialization
         */
        STATE_INIT,

        /**
         * @brief device should spend most time here.
         * Sit in IDLE until connected to charger or the RTC alarms.
         */
        STATE_IDLE,

        /**
         * @brief check the patient's movement and position using the actigraph.
         * If the patient is in a good position, move to measure BP,
         * otherwise, schedule a retry and go back to idle.
         */
        STATE_CHECK_ACTIGRAPH,

        /**
         * @brief utilize the BP sensor to estimate BP.
         * If the BP is elevated, move to an ALERT state.
         */
        STATE_MEASURE_BP,

        /**
         * @brief alert the user about concerning elevated / deelevated BP.
         */
        STATE_ALERT_WARNING,

        /**
         * @brief alert the user about critically elevated / deelevated BP.
         */
        STATE_ALERT_CRITICAL,

        /**
         * @brief enter on device charging, activate BT communication to app
         */
        STATE_CHARGING,
    };

    SystemState currentState = STATE_INIT;
    BleManager &bt;
    Sensors &sensors;
    Indicators &indicators;
    Power &power;
    Log &log;
    FlashLog &flash;

    void handleInit();
    void handleIdle();
    void handleCheckActigraph();
    void handleMeasureBP();
    void handleAlertWarning();
    void handleAlertCritical();
    void handleCharging();
};

#endif