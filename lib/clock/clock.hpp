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

/// @brief functions to manipulate time and interrupts
class Time {
  public:
    Time();

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
    bool setTime(uint64_t time);

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
    uint64_t _time;
};