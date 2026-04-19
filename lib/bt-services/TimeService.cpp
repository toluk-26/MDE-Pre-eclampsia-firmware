/**
 * @file TimeService.cpp
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

    // time characteristic
    _time.setUuid(UUID_CHR_TIME);
    _time.setProperties(CHR_PROPS_WRITE |
                        CHR_PROPS_READ); // TODO: move to write only?
    _time.setWriteCallback(TimeService::time_cb, true);
    _time.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _time.setFixedLen(sizeof(uint64_t));
    _time.setUserDescriptor("TIME");
    VERIFY_STATUS(_time.begin());
    uint64_t currentTime = rtc.getTime();
    _time.write(&currentTime, sizeof(uint64_t));

    // timezone characteristic
    _tz.setUuid(UUID_CHR_TIMEZONE);
    _tz.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY | CHR_PROPS_READ);
    _tz.setWriteCallback(TimeService::tz_cb, true);
    _tz.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _tz.setFixedLen(sizeof(uint8_t));
    _tz.setUserDescriptor("TZ");
    VERIFY_STATUS(_tz.begin());
    _tz.write8(rtc.getTz());

    return ERROR_NONE;
}

void TimeService::time_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                          uint8_t *data, uint16_t len) {
    if (len != sizeof(uint64_t)) {
        LOGE("Time received size is wrong. size received: %u", len);
        return;
    }

    // save it to rtc
    uint64_t time;
    memcpy(&time, data, len);
    rtc.setTime(time); // save the time to the rtc
    LOGV("Unix Time Received: %u%u", static_cast<uint32_t>(time >> 32),
         static_cast<uint32_t>(time));
}

void TimeService::tz_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                        uint8_t *data, uint16_t len) {
    if (len != sizeof(int8_t)) {
        LOGE("Timezone received size is wrong. size received: %u", len);
        return;
    }
    // save to rtc
    int8_t tz;
    memcpy(&tz, data, len);
    rtc.setTz(tz);

    // echo
    TimeService &svc = (TimeService &)chr->parentService();
    svc._tz.notify8(rtc.getTz());
    LOGV("Timezone Received: %d", tz);
}
