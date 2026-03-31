#include "ConfigService.hpp"
#include "log.hpp"
constexpr uint16_t sizeOfThreshold = 2 * sizeof(int);

err_t ConfigService::begin() {
    // Invoke base class begin()
    VERIFY_STATUS(BLEService::begin());

    // pid characteristic
    _pid.setUuid(UUID_CHR_PID);
    _pid.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY | CHR_PROPS_WRITE);
    _pid.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _pid.setFixedLen(sizeof(int)); // TODO: verify size. or is it a string?
    _pid.setUserDescriptor("PID");
    _pid.setWriteCallback(ConfigService::pid_cb, true);
    VERIFY_STATUS(_pid.begin());

    // diastolic characteristic
    _diastolic.setUuid(UUID_CHR_DIASTOLIC);
    _diastolic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                             CHR_PROPS_WRITE);
    _diastolic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _diastolic.setFixedLen(sizeOfThreshold); // TODO: verify size
    _diastolic.setUserDescriptor("Diastolic Thresholds");
    _diastolic.setWriteCallback(ConfigService::diastolic_cb, true);
    VERIFY_STATUS(_diastolic.begin());

    // systolic characteristic
    _systolic.setUuid(UUID_CHR_SYSTOLIC);
    _systolic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY |
                            CHR_PROPS_WRITE);
    _systolic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _systolic.setFixedLen(sizeOfThreshold); // TODO: verify size
    _systolic.setUserDescriptor("Systolic Thresholds");
    _systolic.setWriteCallback(ConfigService::systolic_cb, true);
    VERIFY_STATUS(_systolic.begin());

    // new patient characteristic
    _newPatient.setUuid(UUID_CHR_NEW_PATIENT);
    _newPatient.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
    _newPatient.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _newPatient.setFixedLen(sizeof(bool));
    _newPatient.setUserDescriptor("New Patient Trigger");
    _newPatient.setWriteCallback(ConfigService::newPatient_cb, true);
    VERIFY_STATUS(_newPatient.begin());

    // demo mode characteristic
    _demoMode.setUuid(UUID_CHR_DEMO_MODE);
    _demoMode.setProperties(CHR_PROPS_READ);
    _demoMode.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _demoMode.setFixedLen(sizeof(uint8_t));
    _demoMode.setUserDescriptor("Demo Mode Enabled");
    VERIFY_STATUS(_demoMode.begin());
#ifdef DEBUG
    _demoMode.write8(true);
#else
    _demoMode.write8(false);
#endif

    return ERROR_NONE;
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
#ifdef DEBUG
    String msg;
    for (uint16_t i = 0; i < len; i++)
        msg += (data[i]);
    LOGV("Diastolic Received: %s", msg);
#endif

    if (len != sizeof(sizeOfThreshold)) {
        LOGE("Diastolic received size is wrong. size received: %u", len);
        return;
    }
    int min, max;
    // TODO: parse data
}

void ConfigService::systolic_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                uint8_t *data, uint16_t len) {
#ifdef DEBUG
    String msg;
    for (uint16_t i = 0; i < len; i++)
        msg += (data[i]);
    LOGV("Systolic Received: %s", msg);
#endif

    if (len != sizeof(sizeOfThreshold)) {
        LOGE("Systolic received size is wrong. size received: %u", len);
        return;
    }

    int min, max;
    // TODO: parse data
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
