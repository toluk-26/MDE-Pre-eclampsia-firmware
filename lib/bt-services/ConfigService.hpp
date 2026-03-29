/**
 * @file ConfigService.hpp
 * @author Tolu Kolade
 * @brief
 * @date March 3, 2026
 */

#include <BLECharacteristic.h>
#include <BLEService.h>

#define UUID_CONFIG_SERVICE "043f0000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_PID "043f0000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_DIASTOLIC"043f0000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_SYSTOLIC "043f0000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_NEW_PATIENT "043f0000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_DEMO_MODE "043f0000-7bdb-4430-a1b9-e7d26fb2b981"

class ConfigService : public BLEService {
  public:
    /// @brief sets Service UUID
    ConfigService() : BLEService(UUID_CONFIG_SERVICE){};
    /// @brief sets characteristics properties
    virtual err_t begin();

  protected:
    BLECharacteristic _pid;  // patient id
    BLECharacteristic _diastolic;  // diastolic min/max threshold
    BLECharacteristic _systolic;  // systolic min/max threshold
    BLECharacteristic _newPatient;  // new patient trigger
    BLECharacteristic _demoMode; // is this device in demo mode?

    static void pid_cb(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data,
                       uint16_t len);
    static void diastolic_cb(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data,
                       uint16_t len);
    static void systolic_cb(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data,
                       uint16_t len);
    static void newPatient_cb(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data,
                       uint16_t len);
};