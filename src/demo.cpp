/**
 * @file demo.cpp
 * @author Samuel Thompson
 * @brief demo program for presenting project at the MDE Expo
 * @date 04/17/2026
 */

// for BLE functionality
#include "StreamController.hpp"
#include "TransferController.hpp"
#include "bt.hpp"

// for actigraph usage
#include "actigraph.hpp"

// for flash memory usage and bp logging
#include "flashlog.hpp"
#include "log.hpp"

// standard header files
#include <arduino.h>
#include <vector>

#define BUZZER_PIN 0
#define BUZZER_HZ 650
#define PPG_STALL_MS 3000

// function prototypes
void handleIdle();
void handleRunActigraph();
void handleSimulatedPPG();
void handleBpRead();
void handleBuzzerAlert();
void initBpLog();

enum DemoState { IDLE, RUN_ACTIGRAPH, SIMULATED_PPG, BP_READ, BUZZER_ALERT };
DemoState state = IDLE;

actigraph ak(5, 1);
TransferController tcrtl;
StreamController scrtl;

void setup() {

    pinMode(BUZZER_PIN, OUTPUT);

    bt.begin();  // bluetooth
    mem.begin(); // flash memory and data logging
    initBpLog(); // initialize flash with random bp data for demo

    /**
     * @todo do bt connection setup stuff here
     */
}

void loop() {

    switch (state) {
    case IDLE: {
        handleIdle();
        break;
    }
    case RUN_ACTIGRAPH: {
        handleRunActigraph();
        break;
    }
    case SIMULATED_PPG: {
        handleSimulatedPPG();
        break;
    }
    case BP_READ: {
        handleBpRead();
        break;
    }
    case BUZZER_ALERT: { // run BUZZER_ALERT for a couple seconds
        handleBuzzerAlert();
        break;
    }
    default: {
        state = IDLE;
        break;
    }
    }
}

// state machine handlers
void handleIdle() {
    /** @todo wait in IDLE until an app button is pressed to start the
       sequence.*/
};

// Run the actigraph and send the result to the app. if the result is good, move
// to the next state.
void handleRunActigraph() {
    bool movementGood = ak.run(); // run the actigraph

    /** app reset */
    if (scrtl.isReset()) {
        state = IDLE;
    }

    // transmit ak result to app
    if (bt.calibrateService.stream_flag) {
        scrtl.run(uint64_t(movementGood));
    }

    /** @todo make sure this doesnt pass without sending*/
    if (movementGood) {
        state = SIMULATED_PPG;
        scrtl.nextStep();
    }
};

// stall for PPG_STALL_MS milliseconds, to simulate a PPG reading
void handleSimulatedPPG() {

    uint32_t timeStamp = millis();
    while (millis() < timeStamp + PPG_STALL_MS) {

        /** app reset */
        if (scrtl.isReset()) {
            state = IDLE;
        }
    }

    state = BP_READ;
    scrtl.nextStep();
};

// read flashlog BP data and send over BT
void handleBpRead() {
    // reset controller to start at the beginning of flash and send over BT
    tcrtl.begin();
    while (!tcrtl.isDone()) { // run until all data sent over BT

        /** app reset */
        if (scrtl.isReset()) {
            state = IDLE;
        }

        tcrtl.run();
    }
    state = BUZZER_ALERT;
    scrtl.nextStep();
};

// run the buzzer for 5 seconds, toggling at .5 second intervals
void handleBuzzerAlert() {

    const long interval = 1000; // 1 second
    uint8_t count = 0;
    unsigned long lastBuzz = millis();

    while (count < 5) {

        /** app reset */
        if (scrtl.isReset()) {
            state = IDLE;
        }

        if (millis() - lastBuzz >= interval) {
            tone(BUZZER_PIN, BUZZER_HZ, 500); // play for 500ms
            lastBuzz = millis();              // reset timestamp
            count++;                          // increment count
        }
    }

    state = IDLE;
    scrtl.nextStep();
};

void initBpLog() {
    uint8_t heartrate = 80;
    uint8_t code = 0; /** @todo what should this be for the demo?*/
    uint8_t diastolic, systolic;

    // build random set of data for storing in flash
    for (int i = 0; i < 50; i++) {
        if (random(0, 1000) < 250)
            diastolic = 70 + (1 * random(0, 3));
        else {
            diastolic = 70 - (1 * random(0, 3));
        }

        if (random(0, 1000) < 250)
            systolic = 110 + (1 * random(0, 5));
        else {
            systolic = 110 - (1 * random(0, 5));
        }
        Log::sensor(heartrate, diastolic, systolic, code);
    }
    // always high concern values for the last entry in flash
    Log::sensor(heartrate, diastolic = 100, systolic = 150, code);
}
