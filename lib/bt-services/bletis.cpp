/**
 * @file bletis.cpp
 * @author Tolu Kolade
 * @brief Time Update Service
 * @date December 23, 2025
 */

#include "bletis.hpp"

BLETis::BLETis() : BLEService(UUID_TIME_SERVICE) {}

err_t BLETis::begin() {
    // Invoke base class begin()
    VERIFY_STATUS(BLEService::begin());

    _time.setUuid(UUID_CHR_TIME);
    _time.setProperties(CHR_PROPS_WRITE |
                        CHR_PROPS_READ); // removed | CHR_PROPS_WRITE_WO_RESP
    _time.setWriteCallback(BLETis::bletis_time_cb, true);

    _time.setPermission(
        SECMODE_OPEN, // TODO: change first arg to SECMODE_NO_ACCESS
        SECMODE_OPEN);
    _time.setFixedLen(4);
    _time.setUserDescriptor("TIME"); // TODO: Remove

    VERIFY_STATUS(_time.begin());

    _tz.setUuid(UUID_CHR_TIMEZONE);
    _tz.setProperties(CHR_PROPS_WRITE | CHR_PROPS_READ);
    _tz.setWriteCallback(BLETis::bletis_tz_cb, true);

    _tz.setPermission(SECMODE_OPEN, SECMODE_OPEN); // can read and write
    _tz.setFixedLen(1);
    _tz.setUserDescriptor("TZ"); // TODO: Remove

    VERIFY_STATUS(_tz.begin());

    _tz.write8(0); // TODO: replace with saved timezone

    return ERROR_NONE;
}

void BLETis::bletis_time_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                            uint8_t *data, uint16_t len) {
    // BLETis &svc = (BLETis &)chr->parentService(); // only need this if you
    // need to access a var inside the class
    uint32_t time;
    if (len != sizeof(time)) {
#ifdef DEBUG
        Serial.println("ERROR: time received size is wrong");
#endif
        return;
    }
    memcpy(&time, data, len);
    // TODO: save the time to the rtc

#ifdef DEBUG
    Serial.printf("STATUS: Unix Time Received > \"%x\"\n", time);
#endif
}

void BLETis::bletis_tz_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                          uint8_t *data, uint16_t len) {
    int8_t tz;
    if (len != sizeof(tz)) {
#ifdef DEBUG
        Serial.println("ERROR: time received size is wrong");
#endif
        return;
    }
    memcpy(&tz, data, len);
    // TODO: save the tz to something

    BLETis &svc = (BLETis &)chr->parentService();
    svc._tz.write8(tz + 1); // TODO: remove

#ifdef DEBUG
    Serial.printf("STATUS: Timezone Received > \"%d\"\n", tz);
#endif
}
