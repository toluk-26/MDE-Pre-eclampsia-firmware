#pragma once

#include "BLECharacteristic.h"
#include "BLEService.h"
#include "bluefruit_common.h"

#define UUID_TIME_SERVICE "043f0000-0ff5-45d1-9502-db9d40757da2"
#define UUID_CHR_TIME "043f0001-0ff5-45d1-9502-db9d40757da2"
#define UUID_CHR_TIMEZONE "043f0002-0ff5-45d1-9502-db9d40757da2"

class BLETis : public BLEService {
  public:
    BLETis();
    virtual err_t begin();

  protected:
    BLECharacteristic _time;
    BLECharacteristic _tz;

    static void bletis_time_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                               uint8_t *data, uint16_t len);
    static void bletis_tz_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                           uint8_t *data, uint16_t len);
};