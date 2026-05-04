/**
 * @file StreamController.hpp
 * @author Tolu Kolade
 * @brief StreamController class. public wrapper for CalibrateService. use this
 * to send live sensor data to the app.
 * @date March 31, 2026
 * @example check out TEST_bt.cpp on how to use it and check if the stream flag
 * is enabled.
 */

#pragma once

#include "bt.hpp"

class StreamController {
  public:
    /// @brief does nothing.
    void begin();

    /// @brief prepares the value and sends it to calibrate chr
    void run(uint64_t value);

    /// @brief make the demo go to the next step
    void nextStep();

    /// @brief a reset has been received!
    bool isReset();
};