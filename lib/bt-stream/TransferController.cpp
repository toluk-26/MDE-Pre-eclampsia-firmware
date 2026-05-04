/**
 * @file TransferController.cpp
 * @author Tolu Kolade
 * @brief TransferController implementation. public wrapper for TransferService.
 * use this to control sending data from the memory to the ble service
 * @date March 30, 2026
 */

#include "TransferController.hpp"
#include "log.hpp"

void TransferController::begin() {
    _done = false;
    _currAddr = LOG_O;
    _numofEntries = 0;
    LOGV3("Begin Reading");
}

SensorPayload TransferController::run() {
    if (_done) return SensorPayload();

    DataHdr header;
    std::vector<uint8_t> payload;
    // get data
    if (!mem.read(_currAddr, header, payload)) {
        LOGV3("Finished Reading");
        _done = true;
        bt.transferService.transfer_flag = false;
        bt.transferService.sendSize(_numofEntries);
        return SensorPayload();
    }

    // reserve size
    std::vector<uint8_t> data;
    uint32_t dataSize = sizeof(DataHdr) + payload.size();
    data.reserve(dataSize);

    // prepare and move data
    const uint8_t *hdrBytes = reinterpret_cast<const uint8_t *>(&header);
    data.insert(data.end(), hdrBytes, hdrBytes + sizeof(DataHdr));
    data.insert(data.end(), payload.begin(), payload.end());

    // send data
    bt.transferService.sendData(data);

    // prepare for next loop
    _currAddr += dataSize;
    _numofEntries++;

    SensorPayload vals;
    memcpy(&vals, payload.data(), sizeof(SensorPayload));
    return vals;
}

bool TransferController::isDone() { return _done; }
