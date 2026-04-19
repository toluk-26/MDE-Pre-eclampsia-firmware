/**
 * @file ConfigService.hpp
 * @author Tolu Kolade
 * @brief configuration service class. used to package and parse
 * configuration data. also has uuids
 * @date March 29, 2026
 */

#include <BLECharacteristic.h>
#include <BLEService.h>

#include "flashlog.hpp"

#define UUID_CONFIG_SERVICE "32610000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_DEMO_MODE "32610001-7bdb-4430-a1b9-e7d26fb2b981"
// #define UUID_CHR_NEW_PATIENT "32610002-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_PID "32610003-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_THRESHOLDS "32610004-7bdb-4430-a1b9-e7d26fb2b981"
// #define UUID_CHR_SYSTOLIC "32610005-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_COEFFICIENTS "32610006-7bdb-4430-a1b9-e7d26fb2b981"
// #define UUID_CHR_SYSTOLIC_C "32610007-7bdb-4430-a1b9-e7d26fb2b981"

class ConfigService : public BLEService {
  public:
    /// @brief sets Service UUID
    ConfigService() : BLEService(UUID_CONFIG_SERVICE) {};
    /// @brief sets characteristics properties
    virtual err_t begin();
    void saveConfig();

    bool flag_newpid = false;
    uint32_t pid;

  protected:
    BLECharacteristic _demoMode;   // is this device in demo mode?
    // BLECharacteristic _newPatient; // new patient trigger
    BLECharacteristic _pid;        // patient id
    BLECharacteristic _thresholds; // diastolic min/max threshold
    // BLECharacteristic _systolic;   // systolic min/max threshold
    BLECharacteristic _coefficients; // diastolic m/b coefficients
    // BLECharacteristic _systolicCoefficients;   // systolic m/b coefficients

    static void newPatient_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                              uint8_t *data, uint16_t len);
    static void pid_cb(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data,
                       uint16_t len);
    static void thresholds_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                              uint8_t *data, uint16_t len);
    // static void systolic_cb(uint16_t conn_hdl, BLECharacteristic *chr,
    //                         uint8_t *data, uint16_t len);
    static void coefficients_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                uint8_t *data, uint16_t len);
    // static void systolic_coefficient_cb(uint16_t conn_hdl, BLECharacteristic
    // *chr,
    //                         uint8_t *data, uint16_t len);

    ConfigPack config;
};