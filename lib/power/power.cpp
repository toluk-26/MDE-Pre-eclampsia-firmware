#include "power.h"
#include "rtc.hpp"

#define BATTERY_PIN A0
#define CHARGER_PIN 5

bool Power::powerSwitchOn() { return true; }      // implement switch detection
bool Power::isBatteryLow() { return false; }      // read battery
bool Power::isCharging() { return false; }        // detect charger
float Power::getBatteryPercent() { return 50.0; } // map ADC voltage

void Power::enterLowPowerMode() {
    /** @todo implement deep sleep, mode must specifically allow the rtc to
     * continue to function*/
}

/**
 * @brief schedules an RTC alarm from the current time + input param
 * @param minutes number of minutes from the current time to schedule alarm
 */
void Power::scheduleRetry(int minutes) {
    uint64_t currentTime = rtc.getTime();
    uint64_t wakeTime = currentTime + (minutes * 60);

    rtc.setAlarm(wakeTime);

#ifdef DEBUG
    Serial.print("Retry scheduled in ");
    Serial.print(minutes);
    Serial.println(" minutes");
#endif
}
