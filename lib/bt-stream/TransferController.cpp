#include "TransferController.hpp"

void TransferController::begin() {
    _done = false;
    _currAddr = LOG_O;
    _numofEntries = 0;
}

void TransferController::run() {
    if (_done) return;
    DataHdr header;
    std::vector<uint8_t> payload;

    // get data
    if (!mem.read(_currAddr, header, payload)) {
#ifdef DEBUG
        Serial.println("STATUS: Finished Reading");
#endif
        _done = true;
        bt.transferService.transfer_flag = false;
        bt.transferService.sendSize(_numofEntries);
        return;
    }

    // send data to ble
    std::vector<uint8_t> data;
    uint32_t dataSize = sizeof(DataHdr) + payload.size();
    data.reserve(dataSize);

    const uint8_t *hdrBytes = reinterpret_cast<const uint8_t *>(&header);
    data.insert(data.end(), hdrBytes, hdrBytes + sizeof(DataHdr));
    data.insert(data.end(), payload.begin(), payload.end());

    bt.transferService.sendData(data);
    _currAddr += dataSize;
    _numofEntries++;
}

bool TransferController::isDone() { return _done; }
