#include "StreamController.hpp"
#include "rtc.hpp"

void StreamController::begin() {}

void StreamController::run(uint64_t value) {
    // get sensor value
    // uint64_t value = rtc.getTime(); // TODO: set to whatever.

    // #ifdef DEBUG
    //     Serial.printf("STATUS: streaming value > %x\n", value);
    // #endif

    // prepare
    std::vector<uint8_t> data;
    data.reserve(sizeof(value));

    // insert data
    const uint8_t *valueBytes = reinterpret_cast<const uint8_t *>(&value);
    data.insert(data.end(), valueBytes, valueBytes + sizeof(value));

    // write it
    bt.calibrateService.sendData(data);
}
