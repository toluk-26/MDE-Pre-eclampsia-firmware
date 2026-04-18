/**
 * @file demo.cpp
 * @author Samuel Thompson
 * @brief demo program for presenting project at the MDE Expo
 * @date 04/17/2026
 */

#include "TransferController.hpp"
#include "actigraph.hpp"
#include "bt.hpp"
#include "flashlog.hpp"
#include "log.hpp"
#include <arduino.h>
#include <vector>

#define BUZZER_PIN 0

enum DemoState { IDLE, ACTIGRAPH, PPG, BP_READ, BUZZER };
DemoState state = IDLE;

actigraph ak(5, 1);
TransferController tcrtl;

void setup() {
    Serial.begin(SERIAL_BAUD); // wait for serial monitor to connect
    while (!Serial && millis() < 2000) {
        ; // do nothing
    }

    bt.begin();  // bluetooth
    mem.begin(); // flash memory and data logging

    uint8_t heartrate = 80;
    uint8_t code = 0; /** @todo what should this be for the demo?*/
    uint8_t diastolic, systolic;

    // build random set of data for storing in flash
    for (int i = 0; i < 100; i++) {
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

    pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    switch (state) {
    case IDLE: {
        Serial.println("Trying to connect to Bluetooth...");
        if (Bluefruit.connected() > 0) {
            Serial.println("Connected to Bluetooth!");
            state = ACTIGRAPH;
        }
        break;
    }

    case ACTIGRAPH: {
        Serial.println("Running Actigraph...");
        bool movementGood = ak.run();

        std::vector<uint8_t> data;
        data = {uint8_t(movementGood ? 1 : 0)};
        bt.transferService.sendData(data);

        if (movementGood) {
            state = PPG;
        }
        break;
    }

    case PPG: {
        Serial.println("Simulating PPG sensor...");

        unsigned long notedTime = millis();
        while (millis() < notedTime + 5000) { // stall for ... milliseconds
            ;
        }
        if (bt.transferService.transfer_flag) {
            bt.transferService.transfer_flag = false;
            state = ACTIGRAPH;
        }
        state = BP_READ;
        break;
    }

    case BP_READ: {
        Serial.println("reading logged BP and sending over BT");

        // reset controller to start beginning of flash and send over BT
        tcrtl.begin();
        while (!tcrtl.isDone()) { // run until all data sent over BT
            tcrtl.run();
        }
        // for resetting demo sequence
        if (bt.transferService.transfer_flag) {
            bt.transferService.transfer_flag = false;
            state = ACTIGRAPH;
        }
        state = BUZZER;
        break;
    }
    case BUZZER: { // run buzzer for a couple seconds
        Serial.println("Running buzzer");

        unsigned long notedTime = millis();
        while (millis() < notedTime + 5000) { // stall for ... milliseconds
            tone(BUZZER_PIN, 650, 500);       // play tone at 650 hz for 0.5s
            noTone(BUZZER_PIN);               // stop tone
            delay(500);                       // for 0.5s
        }

        // for resetting demo sequence
        if (bt.transferService.transfer_flag) {
            bt.transferService.transfer_flag = false;
            state = ACTIGRAPH;
        }
        state = ACTIGRAPH;
        break;
    }
    default: {
        Serial.println("Invalid state");
        state = IDLE;
        break;
    }
    }
}
