# clock
set alarm in unix time. i can make a layer to go into human format, but it should be in unix time. you can use [this](https://www.unixtimestamp.com/) website to help you convert in between.

the beginning time value is jan 1, 2026 at 12:00:00am. please don't mess with the the prescaler value unless you need to test overflow. overflow will occur every 25 days 06hr 32min 32sec.

call 'clock.getTime()' to get the unix time. callback functions for the alarm and snooze shouldn't be necessary, since it will hopefully be "waking up".

**NOTE:** low power mode isn't set yet.
# peek
this section is to help me write the code
```c
/** @brief RTC tasks. */
typedef enum
{
    NRF_RTC_TASK_START            /**< Start. */
    NRF_RTC_TASK_STOP              /**< Stop. */
    NRF_RTC_TASK_CLEAR             /**< Clear. */
    NRF_RTC_TASK_TRIGGER_OVERFLOW /**< Trigger overflow. */
} nrf_rtc_task_t;

/** @brief RTC events. */
typedef enum
{
    NRF_RTC_EVENT_TICK         /**< Tick event. */
    NRF_RTC_EVENT_OVERFLOW    /**< Overflow event. */
    NRF_RTC_EVENT_COMPARE_0   /**< Compare 0 event. */
    NRF_RTC_EVENT_COMPARE_1    /**< Compare 1 event. */
    NRF_RTC_EVENT_COMPARE_2    /**< Compare 2 event. */
    NRF_RTC_EVENT_COMPARE_3   /**< Compare 3 event. */
} nrf_rtc_event_t;

/** @brief RTC interrupts. */
typedef enum
{
    NRF_RTC_INT_TICK_MASK     /**< RTC interrupt from tick event. */
    NRF_RTC_INT_OVERFLOW_MASK /**< RTC interrupt from overflow event. */
    NRF_RTC_INT_COMPARE0_MASK /**< RTC interrupt from compare event on channel 0. */
    NRF_RTC_INT_COMPARE1_MASK /**< RTC interrupt from compare event on channel 1. */
    NRF_RTC_INT_COMPARE2_MASK /**< RTC interrupt from compare event on channel 2. */
    NRF_RTC_INT_COMPARE3_MASK /**< RTC interrupt from compare event on channel 3. */
} nrf_rtc_int_t;
```