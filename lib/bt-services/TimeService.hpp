/**
 * @file bletis.hpp
 * @author Tolu Kolade
 * @brief Time Update Service. two characteristics for the time and timezone
 * @date December 23, 2025
 */

#pragma once

#include "BLECharacteristic.h"
#include "BLEService.h"
#include "bluefruit.h"

#define UUID_TIME_SERVICE "043f0000-0ff5-45d1-9502-db9d40757da2"
#define UUID_CHR_TIME "043f0001-0ff5-45d1-9502-db9d40757da2"
#define UUID_CHR_TIMEZONE "043f0002-0ff5-45d1-9502-db9d40757da2"

/**
 * @brief Time BLE Service. will update RTC time from phone app
 */
class TimeService : public BLEService {
  public:
    /// @brief sets Service UUID
    TimeService();
    /// @brief sets characteristics properties
    virtual err_t begin();

  protected:
    BLECharacteristic _time;
    BLECharacteristic _tz;

    /**
     * @brief time callback function you can trigger a read without writing by
     * writing any amount not 8 bytes. this may be changed so that there is not
     * point to read
     */
    static void time_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                        uint8_t *data, uint16_t len);

    /**
     * @brief timezone callback function
     */
    static void tz_cb(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data,
                      uint16_t len);
};