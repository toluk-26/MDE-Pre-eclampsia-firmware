/**
 * @file clock.cpp
 * @brief implementation of the time interface for the RTC on the nrf52. since
 * the rtc is a counter and not timer, we must be careful.
 * @author Tolu Kolade
 * @date January 3, 2025
 *
 * @todo implement check for out of bound count and update the time.
 * @todo check if snooze works
 */

#include "rtc.hpp"
#include <Arduino.h>
#include <nrf_rtc.h>

#ifdef DEBUG
#include <inttypes.h>
#endif

RTC rtc; // all rtc clock. call clock.tick() as much as possible

static volatile bool alarmFlag = false;    // if the alarm interrupt
static volatile bool snoozeFlag = false;   // if the snooze interrupt
static volatile bool overflowFlag = false; // if the overflow interrupt

// find factor to convert between time and counter count
constexpr uint PRESCALAR_FACTOR = 32768 / (PRESCALER_C + 1);

extern "C" void RTC2_IRQHandler(void) {
    // Check and clear event FIRST
    if (nrf_rtc_event_check(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0)) {
        nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
        alarmFlag = true;
    }
    if (nrf_rtc_event_check(NRF_RTC2, NRF_RTC_EVENT_OVERFLOW)) {
        nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_OVERFLOW);
        overflowFlag = true;
    }
    if (nrf_rtc_event_check(NRF_RTC2, NRF_RTC_EVENT_COMPARE_1)) {
        nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_1);
        snoozeFlag = true;
    }
    // On Cortex-M4, ensure the store completes before exiting ISR
    __DSB();
    __ISB();
}

RTC::RTC() {
    // prep
    nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_STOP);  // stop rtc
    nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_CLEAR); // clear
    nrf_rtc_prescaler_set(NRF_RTC2, PRESCALER_C);       // set prescaler

    // clear events
    nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
    nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_1);
    nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_OVERFLOW);

    // enable overflow interrupt
    nrf_rtc_int_enable(NRF_RTC2, NRF_RTC_INT_OVERFLOW_MASK);

    // TODO: move and enable

    // prepare interrupt
    NVIC_DisableIRQ(RTC2_IRQn);
    NVIC_ClearPendingIRQ(RTC2_IRQn);
    NVIC_SetPriority(RTC2_IRQn, 6);
    NVIC_EnableIRQ(RTC2_IRQn);

    // begin
    nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_START);
}

bool RTC::tick() {
    // check if anything happened
    if (!(alarmFlag || snoozeFlag || overflowFlag)) return false;

    // move flags
    bool o;
    a = alarmFlag;
    s = snoozeFlag;
    o = overflowFlag;
    alarmFlag = snoozeFlag = overflowFlag = false;

    // update time in overflow
    /// @note does what setTime without clearing counter
    if (o) _time += convertCounter(0x1000000); // convert to static value

#ifdef DEBUG
    if (a) Serial.println("STATUS: alarm trigger");
    if (s) Serial.println("STATUS: snooze trigger");
    if (o) Serial.println("STATUS: overflow trigger");
    if (a) setAlarm(getTime() + 60);
#endif

    return true;
}

uint64_t RTC::getTime() {
    return _time + convertCounter(nrf_rtc_counter_get(NRF_RTC2));
}

void RTC::setTime(uint64_t time) {
    // TODO: idk what the nrf code does. we will need to redo the alarm
    _time = time;
    nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_CLEAR);
#ifdef DEBUG
    Serial.print("STATUS: base time updated to ");
    print64(time);
    Serial.print("\n");
#endif
}

int8_t RTC::getTz() {
    // TODO: get tz from the flash and save it to ram
    return _tz;
}

void RTC::setTz(int8_t tz) {
    // TODO: finalize tz plan
    _tz = tz;
}

bool RTC::setAlarm(uint64_t time) {
    if (time < _time) {
#ifdef DEBUG
        Serial.println("ERROR: alarm input time is smaller than base time. "
                       "rejected"); // time must be larger than base time
#endif
        return false;
    }
    if (time < getTime()) {
#ifdef DEBUG
        Serial.println("ERROR: alarm input time is smaller than current time. "
                       "rejected"); // time must be larger than base time
#endif
        return false;
    }

    // enable interrupt
    if (!nrf_rtc_int_enable_check(NRF_RTC2, NRF_RTC_INT_COMPARE0_MASK)) {
        nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
        nrf_rtc_int_enable(NRF_RTC2, NRF_RTC_INT_COMPARE0_MASK);
    }

    // find how much time in the future and convert to count
    uint countercompare = (time - _time) * PRESCALAR_FACTOR;
    nrf_rtc_cc_set(NRF_RTC2, 0, countercompare); // set cc register

#ifdef DEBUG
    Serial.printf("STATUS: alarm set for %u seconds\n", (time - getTime()));
#endif

    return true;
}

uint64_t RTC::convertCounter(uint32_t counter) {
    return counter / PRESCALAR_FACTOR;
}

#ifdef DEBUG
void print64(uint64_t v) {
    uint32_t hi = (uint32_t)(v >> 32);
    uint32_t lo = (uint32_t)(v & 0xFFFFFFFFu);

    if (hi) {
        Serial.print(hi);
        Serial.print(lo < 1000000000UL ? "0" : "");
        Serial.print(lo);
    } else {
        Serial.print(lo);
    }
}
#endif
