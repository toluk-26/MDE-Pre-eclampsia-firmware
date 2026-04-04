/**
 * @file TransferService.cpp
 * @author Tolu Kolade
 * @brief Transfer Service. used to stream packets in flash data block
 * @date March 29, 2026
 * 
 * @todo move data to cccdwritecallback() to use as trigger
 * @todo consider using indicate with no size or size + notify for better speed
 */

#include "TransferService.hpp"
#include "log.hpp"
#include <bluefruit.h>
#include <vector>

err_t TransferService::begin() {
    // Invoke base class begin()
    VERIFY_STATUS(BLEService::begin());

    // trigger characteristic
    _trigger.setUuid(UUID_CHR_TRIGGER);
    _trigger.setProperties(CHR_PROPS_WRITE);
    _trigger.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _trigger.setFixedLen(sizeof(bool));
    _trigger.setUserDescriptor("Trigger");
    _trigger.setWriteCallback(TransferService::trigger_cb, true);
    VERIFY_STATUS(_trigger.begin());

    // data characteristic
    _data.setUuid(UUID_CHR_DATA);
    _data.setProperties(CHR_PROPS_INDICATE);
    _data.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _data.setMaxLen(100);
    _data.setUserDescriptor("Data");
    VERIFY_STATUS(_data.begin());

    // data characteristic
    _size.setUuid(UUID_CHR_DATA_SIZE);
    _size.setProperties(CHR_PROPS_NOTIFY);
    _size.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _size.setFixedLen(sizeof(uint));
    _size.setUserDescriptor("Size");
    VERIFY_STATUS(_size.begin());

    return ERROR_NONE;
}

void TransferService::sendData(const std::vector<uint8_t> &data) {
    this->_data.indicate(data.data(), data.size());
    // LOGV("\tSize: %u", data.size());
}

void TransferService::sendSize(const uint32_t &size) { _size.notify32(size); }

void TransferService::trigger_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                                 uint8_t *data, uint16_t len) {
    if (len != sizeof(bool)) {
        LOGE("Trigger received size is wrongsize is wrong. size received: %u",
             len);

        return;
    }
    TransferService &svc = (TransferService &)chr->parentService();
#ifdef DEBUG
    if (svc.transfer_flag) {
        LOGV("trigger is already triggered");
        return;
    }
#endif
    svc.transfer_flag = true; // set flag
}
