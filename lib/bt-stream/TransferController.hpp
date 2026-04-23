/**
 * @file TransferController.hpp
 * @author Tolu Kolade
 * @brief TransferController implementation. public wrapper for TransferService.
 * use this to control sending data from the memory to the ble service
 * @date March 30, 2026
 * @example check out TEST_bt.cpp on how to use it and check if the stream flag
 * is enabled.
 */

#pragma once

#include "bt.hpp"
#include "flashlog.hpp"

struct SensorPayload {
    uint8_t heartrate;
    uint8_t systolic;
    uint8_t diastolic;
    uint8_t code;
};

class TransferController {
  public:
    /// @brief prepare values in private section
    void begin();

    /// @brief each packet is sent on one loop. almost non-blocking
    SensorPayload run();

    /// @brief check if transfer is done. also updates size character
    bool isDone();

  private:
    bool _done = true;
    uint32_t _currAddr = LOG_O;
    int _numofEntries = 0;
};