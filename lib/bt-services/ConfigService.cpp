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

    ConfigPack c = mem.getConfig();
    config = c;

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
    _thresholds.setUuid(UUID_CHR_THRESHOLDS);
    _thresholds.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                              CHR_PROPS_WRITE);
    _thresholds.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _thresholds.setFixedLen(2 * sizeof(ThresholdData));
    _thresholds.setUserDescriptor("Thresholds");
    _thresholds.setWriteCallback(ConfigService::thresholds_cb, true);
    VERIFY_STATUS(_thresholds.begin());
    // send threshold
    ThresholdData thresholdPacket[2] = {{c.systolic_min, c.systolic_max},
                                        {c.diastolic_min, c.diastolic_max}};
    _thresholds.write(&thresholdPacket, sizeof(thresholdPacket));

    // diastolic coefficient characteristic
    _coefficients.setUuid(UUID_CHR_COEFFICIENTS);
    _coefficients.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                                CHR_PROPS_WRITE);
    _coefficients.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _coefficients.setFixedLen(2 * sizeof(CoefficientData));
    _coefficients.setUserDescriptor("Diastolic Coefficients");
    _coefficients.setWriteCallback(ConfigService::coefficients_cb, true);
    VERIFY_STATUS(_coefficients.begin());
    // send coefficients
    CoefficientData coefficientPacket[2] = {
        {c.diastolic_coeff_m, c.diastolic_coeff_b},
        {c.diastolic_coeff_m, c.diastolic_coeff_b}};
    _coefficients.write(&coefficientPacket, sizeof(coefficientPacket));

    return ERROR_NONE;
}

void ConfigService::saveConfig() {
    ConfigPack &c = config;
    _pid.write32(c.pid);
    ThresholdData thresholdPacket[2] = {{c.systolic_min, c.systolic_max},
                                        {c.diastolic_min, c.diastolic_max}};
    _thresholds.write(&thresholdPacket, 2 * sizeof(thresholdPacket));
    CoefficientData coefficientPacket[2] = {
        {c.diastolic_coeff_m, c.diastolic_coeff_b},
        {c.diastolic_coeff_m, c.diastolic_coeff_b}};
    _coefficients.write(&coefficientPacket, 2 * sizeof(coefficientPacket));

    LOGV("Threshold Values");
    LOGV("\tHigh:\t%u/%u", config.systolic_max, config.diastolic_max);
    LOGV("\tLow:\t%u/%u", config.systolic_min, config.diastolic_min);

    LOGV("Coefficient Values");
    LOGV("\tSystolic:\tS = %f * x + %f", config.systolic_coeff_m,
         config.systolic_coeff_b);
    LOGV("\tDiastolic:\tD = %f * x + %f", config.diastolic_coeff_m,
         config.diastolic_coeff_b);

    mem.configload = c;
    mem.setConfig();
}

// void ConfigService::newPatient_cb(uint16_t conn_hdl, BLECharacteristic *chr,
//                                   uint8_t *data, uint16_t len) {
//     if (len != sizeof(bool)) {
//         LOGE("New Patient received size is wrong");
//         return;
//     }
//     LOGV("New Patient Flag!");
//     // TODO: perform task then notify that task are done
// }

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
    ConfigService &svc = (ConfigService &)chr->parentService();
    svc.pid = pid;
    svc.flag_newpid = true;
}

void ConfigService::thresholds_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                  uint8_t *data, uint16_t len) {
    LOGV("Thresholds Received");
    if (len != 2 * sizeof(ThresholdData)) {
        LOGE("\treceived size is wrong. size received: %u", len);
        return;
    }

    ThresholdData tds[2];
    memcpy(&tds, data, len);
    ConfigService &svc = (ConfigService &)chr->parentService();
    svc.config.systolic_max = tds[0].max;
    svc.config.systolic_min = tds[0].min;
    svc.config.diastolic_max = tds[1].max;
    svc.config.diastolic_min = tds[1].min;

    svc._thresholds.notify(&tds, sizeof(tds));
}

void ConfigService::coefficients_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                    uint8_t *data, uint16_t len) {
    LOGV("Coefficients Received");
    if (len != 2 * sizeof(CoefficientData)) {
        LOGE("\treceived size is wrong. size received: %u", len);
        return;
    }

    CoefficientData cds[2];
    memcpy(&cds, data, len);
    ConfigService &svc = (ConfigService &)chr->parentService();
    svc.config.systolic_coeff_m = cds[0].max;
    svc.config.systolic_coeff_b = cds[0].min;
    svc.config.diastolic_coeff_m = cds[1].max;
    svc.config.diastolic_coeff_b = cds[1].min;

    svc._coefficients.notify(&cds, sizeof(cds));
}
