/**
 * @file ConfigService.cpp
 * @author Tolu Kolade
 * @brief configuration service implementation. used to package and parse
 * configuration data.
 * @date March 29, 2026
 * 
 * @todo combine systolic+diastolic into threshold character
 * @todo combine sys coeff+dia coeff into coefficient character
 * @todo save config data on disconnect
 * @todo use pid write as new patient trigger
 */

#include "ConfigService.hpp"
#include "flashlog.hpp"
#include "log.hpp"

// format of threshold payload
struct ThresholdData {
    uint8_t min, max;
};
// format of threshold payload
struct CoefficientData {
    float min, max;
};

err_t ConfigService::begin() {
    // Invoke base class begin()
    VERIFY_STATUS(BLEService::begin());

    // demo mode characteristic
    _demoMode.setUuid(UUID_CHR_DEMO_MODE);
    _demoMode.setProperties(CHR_PROPS_READ);
    _demoMode.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _demoMode.setFixedLen(sizeof(uint8_t));
    _demoMode.setUserDescriptor("Demo Mode Enabled");
    VERIFY_STATUS(_demoMode.begin());
#ifdef DEBUG
    _demoMode.write8(0x01); // TODO: remove. this should be for demo. i have it
                            // here for console access from menu
#else
    _demoMode.write8(0x00);
#endif

    // new patient characteristic
    _newPatient.setUuid(UUID_CHR_NEW_PATIENT);
    _newPatient.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
    _newPatient.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _newPatient.setFixedLen(sizeof(bool));
    _newPatient.setUserDescriptor("New Patient Trigger");
    _newPatient.setWriteCallback(ConfigService::newPatient_cb, true);
    VERIFY_STATUS(_newPatient.begin());

    ConfigPack c = mem.getConfig();

    // pid characteristic
    _pid.setUuid(UUID_CHR_PID);
    _pid.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY | CHR_PROPS_WRITE);
    _pid.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _pid.setFixedLen(sizeof(int)); // TODO: verify size. or is it a string?
    _pid.setUserDescriptor("PID");
    _pid.setWriteCallback(ConfigService::pid_cb, true);
    VERIFY_STATUS(_pid.begin());
    // send pid
    _pid.write32(c.pid);

    // diastolic characteristic
    _diastolic.setUuid(UUID_CHR_DIASTOLIC);
    _diastolic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                             CHR_PROPS_WRITE);
    _diastolic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _diastolic.setFixedLen(sizeof(ThresholdData));
    _diastolic.setUserDescriptor("Diastolic Thresholds");
    _diastolic.setWriteCallback(ConfigService::diastolic_cb, true);
    VERIFY_STATUS(_diastolic.begin());
    // send threshold
    ThresholdData minmaxpacket = {c.diastolic_min, c.diastolic_max};
    _diastolic.write(&minmaxpacket, sizeof(ThresholdData));

    // systolic characteristic
    _systolic.setUuid(UUID_CHR_SYSTOLIC);
    _systolic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                            CHR_PROPS_WRITE);
    _systolic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _systolic.setFixedLen(sizeof(ThresholdData));
    _systolic.setUserDescriptor("Systolic Thresholds");
    _systolic.setWriteCallback(ConfigService::systolic_cb, true);
    VERIFY_STATUS(_systolic.begin());
    // send threshold
    minmaxpacket = {c.systolic_min, c.systolic_max};
    _systolic.write(&minmaxpacket, sizeof(ThresholdData));

    // diastolic coefficient characteristic
    _diastolicCoefficients.setUuid(UUID_CHR_DIASTOLIC_C);
    _diastolicCoefficients.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                                         CHR_PROPS_WRITE);
    _diastolicCoefficients.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _diastolicCoefficients.setFixedLen(sizeof(CoefficientData));
    _diastolicCoefficients.setUserDescriptor("Diastolic Coefficients");
    _diastolicCoefficients.setWriteCallback(ConfigService::diastolic_cb, true);
    VERIFY_STATUS(_diastolicCoefficients.begin());
    // send coefficients
    CoefficientData coefficientpacket = {c.diastolic_coeff_m,
                                         c.diastolic_coeff_b};
    _diastolicCoefficients.write(&coefficientpacket, sizeof(CoefficientData));

    // systolic coefficient characteristic
    _systolicCoefficients.setUuid(UUID_CHR_SYSTOLIC_C);
    _systolicCoefficients.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                                        CHR_PROPS_WRITE);
    _systolicCoefficients.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _systolicCoefficients.setFixedLen(sizeof(CoefficientData));
    _systolicCoefficients.setUserDescriptor("Systolic Coefficients");
    _systolicCoefficients.setWriteCallback(ConfigService::systolic_cb, true);
    VERIFY_STATUS(_systolicCoefficients.begin());
    // send coefficients
    coefficientpacket = {c.diastolic_coeff_m, c.diastolic_coeff_b};
    _systolicCoefficients.write(&coefficientpacket, sizeof(CoefficientData));

    return ERROR_NONE;
}

void ConfigService::newPatient_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                  uint8_t *data, uint16_t len) {
    if (len != sizeof(bool)) {
        LOGE("New Patient received size is wrong");
        return;
    }
    LOGV("New Patient Flag!");
    // TODO: perform task then notify that task are done
}

void ConfigService::pid_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                           uint8_t *data, uint16_t len) {
#ifdef DEBUG
    String msg;
    for (uint16_t i = 0; i < len; i++)
        msg += (data[i]);
    LOGV("PID %s Received");
#endif

    if (len != sizeof(uint32_t)) {
        LOGE("PID received size is wrong. size received: %u", len);
        return;
    }
    uint32_t pid;
    memcpy(&pid, data, len);
    // TODO: save pid
}

void ConfigService::diastolic_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                 uint8_t *data, uint16_t len) {
    LOGV("Diastolic Received");
    if (len != sizeof(ThresholdData)) {
        LOGE("\treceived size is wrong. size received: %u", len);
        return;
    }

    ThresholdData td;
    memcpy(&td, data, len);
    // TODO: save data
}

void ConfigService::systolic_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                uint8_t *data, uint16_t len) {
    LOGV("Systolic Received");
    if (len != sizeof(ThresholdData)) {
        LOGE("\treceived size is wrong. size received: %u", len);
        return;
    }

    ThresholdData td;
    memcpy(&td, data, len);
    // TODO: save data
}

void ConfigService::diastolic_coefficient_cb(uint16_t conn_hdl,
                                             BLECharacteristic *chr,
                                             uint8_t *data, uint16_t len) {
    LOGV("Diastolic Coefficients Received");
    if (len != sizeof(CoefficientData)) {
        LOGE("\treceived size is wrong. size received: %u", len);
        return;
    }

    CoefficientData cd;
    memcpy(&cd, data, len);
    // TODO: save data
}

void ConfigService::systolic_coefficient_cb(uint16_t conn_hdl,
                                            BLECharacteristic *chr,
                                            uint8_t *data, uint16_t len) {
    LOGV("Systolic Coefficients Received");
    if (len != sizeof(CoefficientData)) {
        LOGE("\treceived size is wrong. size received: %u", len);
        return;
    }

    CoefficientData cd;
    memcpy(&cd, data, len);
    // TODO: save data
}
