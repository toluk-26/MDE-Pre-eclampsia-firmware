/**
 * @file clock.hpp
 * @brief declaration of the time interface for the RTC on the nrf52. since the
 * rtc is a counter and not timer, we must be careful.
 * @author Tolu Kolade
 * @date January 3, 2025
 *
 * @todo change setAlarm() param to uint or human time
 */

#pragma once
#include <cstdint>

#define PRESCALER_C 4095 // Max value is 4095

/// @brief functions to manipulate time and interrupts
class Time {
  public:
    Time();

    bool tick();

    /**
     * @brief get the unix time from the rtc
     * @return unix time
     */
    uint64_t getTime();

    /**
     * @brief set the time and starts the rtc. should only be called internally
     * and by bt class
     * @param time unix time
     * @return successfully set time
     */
    void setTime(uint64_t time);

    /**
     * @brief set interrupt for time
     * @param time time of alarm
     * @return successfully set alarm
     */
    bool setAlarm(uint64_t time);

    /**
     * @brief sets new interrupt for some time later. used in cases where sample
     * time is spoiled
     * @return alarm set
     */
    bool snooze();

  private:
    /// @brief holds last unix time. to get actual time, use function to update
    /// from rtc
    /// @note default value is Jan 1, 2026 00:00:00 UTC
    uint64_t _time = 1767243600;

    /// @brief timezone
    uint8_t _tz;

    bool a, s;

    /**
     * @brief converts the rtc precision stuff to seconds
     * @param value counter value
     * @return successfully set time
     */
    uint64_t convertCounter(uint32_t counter);
};

extern Time clock;

#ifdef DEBUG
/**
 * @brief print a uint64_t. for debugging only. no endline
 * @param v value to print
 */
void print64(uint64_t v);
#endif