#include "CalibrateService.hpp"

err_t CalibrateService::begin() {
    VERIFY_STATUS(BLEService::begin());

    // trigger characteristic
    _trigger.setUuid(UUID_CHR_TRIGGER_C);
    _trigger.setProperties(CHR_PROPS_WRITE);
    _trigger.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _trigger.setFixedLen(sizeof(bool));
    _trigger.setUserDescriptor("Trigger");
    _trigger.setWriteCallback(CalibrateService::trigger_cb, true);
    VERIFY_STATUS(_trigger.begin());

    // data characteristic
    _stream.setUuid(UUID_CHR_STREAM);
    _stream.setProperties(CHR_PROPS_NOTIFY);
    _stream.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _stream.setFixedLen(0); // TODO: try 0
    _stream.setUserDescriptor("Data");
    VERIFY_STATUS(_stream.begin());

    return ERROR_NONE;
}

void CalibrateService::sendData(const std::vector<uint8_t> &data) {
    _stream.notify(data.data(), data.size());
#ifdef DEBUG
    for (size_t i = 0; i < data.size(); i++) {
        Serial.printf("%02X ", data[i]);
    }
    Serial.printf("STATUS: size %d\n", data.size());
#endif
}

void CalibrateService::trigger_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                  uint8_t *data, uint16_t len) {
    if (len != sizeof(uint8_t)) {
#ifdef DEBUG
        Serial.println("ERROR: trigger received size is wrong");
#endif
        return;
    }
    CalibrateService &svc = (CalibrateService &)chr->parentService();

    if (svc.stream_flag) {
        svc.stream_flag = false; // disable flag
#ifdef DEBUG
        Serial.println("STATUS: stream trigger flag disabled!");
#endif
    } else {
        svc.stream_flag = true; // set flag
#ifdef DEBUG
        Serial.println("STATUS: stream trigger flag!");
#endif
    }
}
