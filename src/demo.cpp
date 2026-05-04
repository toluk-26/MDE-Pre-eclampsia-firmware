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
#define SYSTOLIC_THRESHOLD 145
#define DIASTOLIC_THRESHOLD 95

// function prototypes
void handleIdle();
void handleRunActigraph();
void handleSimulatedPPG();
void handleBpRead();
void handleBuzzerAlert();
void initBpLog();

enum DemoState { IDLE, RUN_ACTIGRAPH, SIMULATED_PPG, BP_READ, BUZZER_ALERT };
DemoState state = IDLE;

Actigraph ak(5, 1);
TransferController tcrtl;
StreamController scrtl;

void setup() {

    // Serial.begin(SERIAL_BAUD); // startup serial monitor
    // while (!Serial) {
    //     ;
    // }

    pinMode(BUZZER_PIN, OUTPUT);
    mem.begin(); // flash memory and data logging
    delay(3000);

    // Load bp demo data here
    // initBpLog(); // initialize flash with random bp data for demo

    bt.begin(); // bluetooth
    // mem.printMeta();
    // mem.printConfig();
    // mem.dumpData();
    // mem.dump();
}

void loop() {
    if (bt.disconnectFlag) state = IDLE;

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
    if (scrtl.isReset()) state = RUN_ACTIGRAPH;
};

unsigned long startPPG;

// Run the actigraph and send the result to the app. if the result is good, move
// to the next state.
void handleRunActigraph() {
    bool movementGood = ak.run(); // run the actigraph

    /** app reset */
    if (scrtl.isReset()) {
        state = IDLE;
        LOGV("Going to Idle");
        return;
    }

    // transmit ak result to app
    if (bt.calibrateService.stream_flag) {
        scrtl.run(uint64_t(movementGood));
    }

    /** @todo make sure this doesnt pass without sending*/
    if (movementGood) {
        state = SIMULATED_PPG;
        startPPG = millis();
        scrtl.nextStep();
        LOGV("Going to SIMULATED_PPG");
    }
};

bool firstRun;
// stall for PPG_STALL_MS milliseconds, to simulate a PPG reading
void handleSimulatedPPG() {
    if (scrtl.isReset()) {
        state = IDLE;
        LOGV("Going to Idle");
        return;
    }

    if (millis() - startPPG <= PPG_STALL_MS) {
        /** app reset */
        state = BP_READ;
        scrtl.nextStep();
        // prepare for bpread
        firstRun = true;
        LOGV("Going to BP_READ");
    }
};

unsigned long lastBuzz;
uint8_t count;

// read flashlog BP data and send over BT
void handleBpRead() {
    if (scrtl.isReset()) {
        state = IDLE;
        LOGV("Going to idle");
        return;
    }

    // reset controller to start at the beginning of flash and send over BT

    if (firstRun) {
        tcrtl.begin();
        firstRun = false;
    }
    if (!tcrtl.isDone()) { // run until all data sent over BT
        SensorPayload v = tcrtl.run();
        if (v.systolic > SYSTOLIC_THRESHOLD ||
            v.diastolic > DIASTOLIC_THRESHOLD) {
            scrtl.nextStep();
            LOGV("Going to BUZZER_ALERT");
            state = BUZZER_ALERT; // next state
            count = 0;
        }
    }
};

// run the buzzer for 5 seconds, toggling at .5 second intervals
void handleBuzzerAlert() {
    if (scrtl.isReset()) {
        state = IDLE;
        LOGV("Going to Idle");
        return;
    }

    const long interval = 1000; // 1 second

    if (count < 5) {
        if (millis() - lastBuzz >= interval) {
            tone(BUZZER_PIN, BUZZER_HZ, 500); // play for 500ms
            lastBuzz = millis();              // reset timestamp
            count++;                          // increment count
            LOGV("Buzz");
        }
    }
};

void initBpLog() {
    mem.cleanAll();

    ConfigPack c = {
        .pid = 7, // minutes is all i can spare
        .diastolic_min = 0,
        .diastolic_max = DIASTOLIC_THRESHOLD,
        .diastolic_coeff_m = 1.0,
        .diastolic_coeff_b = 0.0,
        .systolic_min = 80, // sys min
        .systolic_max = SYSTOLIC_THRESHOLD,
        .systolic_coeff_m = 1.0,
        .systolic_coeff_b = 0.0,
    };

    mem.configload = c;
    mem.setConfig();

    uint8_t heartrate = 80;
    uint8_t code = 0; /** @todo what should this be for the demo?*/
    uint8_t diastolic, systolic;

    // build random set of data for storing in flash
    for (int i = 0; i < 50; i++) {
        if (random(0, 1000) < 150)
            diastolic = 70 + i / 2 + (random(0, 5));
        else {
            diastolic = 70 + i / 2 - (random(0, 3));
        }

        if (random(0, 1000) < 150)
            systolic = 110 + i + (random(0, 10));
        else {
            systolic = 110 + i - (random(0, 5));
        }
        Log::sensor(heartrate, systolic, diastolic, code);
    }
    // always high concern values for the last entry in flash
    Log::sensor(heartrate, systolic = 150, diastolic = 100, code);
}
