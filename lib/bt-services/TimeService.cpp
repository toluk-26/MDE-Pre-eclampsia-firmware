/**
 * @file bletis.cpp
 * @author Tolu Kolade
 * @brief Time Update Service
 * @date December 23, 2025
 */

#include "TimeService.hpp"
#include "log.hpp"
#include "rtc.hpp"

TimeService::TimeService() : BLEService(UUID_TIME_SERVICE) {}

err_t TimeService::begin() {
    // Invoke base class begin()
    VERIFY_STATUS(BLEService::begin());

    _time.setUuid(UUID_CHR_TIME);
    _time.setProperties(CHR_PROPS_WRITE | CHR_PROPS_READ);
    _time.setWriteCallback(TimeService::time_cb, true);

    _time.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _time.setFixedLen(sizeof(uint64_t));
    _time.setUserDescriptor("TIME"); // TODO: Remove

    VERIFY_STATUS(_time.begin());
    uint64_t currentTime = rtc.getTime();
    _time.write(&currentTime, sizeof(uint64_t));

    _tz.setUuid(UUID_CHR_TIMEZONE);
    _tz.setProperties(CHR_PROPS_WRITE | CHR_PROPS_READ);
    _tz.setWriteCallback(TimeService::tz_cb, true);

    _tz.setPermission(SECMODE_OPEN, SECMODE_OPEN); // can read and write
    _tz.setFixedLen(sizeof(uint8_t));
    _tz.setUserDescriptor("TZ"); // TODO: Remove

    VERIFY_STATUS(_tz.begin());

    _tz.write8(rtc.getTz());

    return ERROR_NONE;
}

void TimeService::time_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                          uint8_t *data, uint16_t len) {
    TimeService &svc = (TimeService &)chr->parentService();
    uint64_t time, currentTime;
    if (len != sizeof(time)) {
        LOGE("Time received size is wrong. size received: %u", len);
        currentTime = rtc.getTime();
        svc._time.write(&currentTime, sizeof(uint64_t)); // echo back
        return;
    }
    memcpy(&time, data, len);
    rtc.setTime(time); // save the time to the rtc
    LOGV("Unix Time Received: %d%d", static_cast<uint32_t>(time >> 32),
         static_cast<uint32_t>(time));

    // TODO: conditional on set time?
    currentTime = rtc.getTime();
    svc._time.write(&currentTime, sizeof(uint64_t)); // echo back
}

void TimeService::tz_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                        uint8_t *data, uint16_t len) {
    int8_t tz;
    if (len != sizeof(tz)) {
        LOGE("Timezone received size is wrong. size received: %u", len);
        return;
    }
    memcpy(&tz, data, len);
    rtc.setTz(tz);

    TimeService &svc = (TimeService &)chr->parentService();
    svc._tz.write8(rtc.getTz());

    LOGV("Timezone Received: %d", tz);
}
