/**
 * @file CalibrateService.hpp
 * @author Tolu Kolade
 * @brief calibrate service implementation. intended to be used by a controller to send live
 * data of a sensor
 * @date March 31, 2026
 * @todo remove trigger and use setCccdWriteCallback()
 */

#include "CalibrateService.hpp"
#include "log.hpp"

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

    // print data to serial monitor
#ifdef DEBUG
    String msg;
    for (uint i = 0; i < data.size(); i++) {
        msg += String(data[i], HEX);
    }
    LOGV3("Sending 0x%s, size:%d", msg.c_str(), data.size());
#endif
}

void CalibrateService::trigger_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                  uint8_t *data, uint16_t len) {
    if (len != sizeof(uint8_t)) {
        LOGE("trigger received size is wrong");
        return;
    }
    // get class
    CalibrateService &svc = (CalibrateService &)chr->parentService();

    if (svc.stream_flag) {
        svc.stream_flag = false; // disable flag
        LOGV("Stream trigger flag disabled!");
    } else {
        svc.stream_flag = true; // set flag
        LOGV("Stream trigger flag enabled!");
    }
}

void CalibrateService::startTransfer_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                      uint16_t value) {
    if (value & 0x0001) {
        // notifications enabled
    }
    if (value & 0x0002) {
        // indications enabled
    }
}