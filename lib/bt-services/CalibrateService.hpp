#pragma once

#include <BLECharacteristic.h>
#include "BLEService.h"
#include "bluefruit.h"
#include <vector>

#define UUID_CALIBRATE_SERVICE "c16e0000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_TRIGGER_C "c16e0001-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_STREAM "c16e0002-7bdb-4430-a1b9-e7d26fb2b981"

/**
 * @brief Time BLE Service. will update RTC time from phone app
 */
class CalibrateService : public BLEService {
  public:
    /// @brief sets Service UUID
    CalibrateService() : BLEService(UUID_CALIBRATE_SERVICE) {};
    /// @brief sets characteristics properties
    virtual err_t begin();

    bool stream_flag = false;
    void sendData(const std::vector<uint8_t> &data);

  protected:
    BLECharacteristic _trigger;
    BLECharacteristic _stream;

    static void trigger_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                           uint8_t *data, uint16_t len);
};