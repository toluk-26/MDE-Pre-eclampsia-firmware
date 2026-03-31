/**
 * @file flashlog.cpp
 * @author Tolu Kolade
 * @date December 27, 2025
 * @todo missing function implementations
 * @todo split header guarded blocks into separate .cpp files
 */

#include "flashlog.hpp"
#include "rtc.hpp"

FlashLog::FlashLog()
    : _SPI_2(NRF_SPIM0, PIN_QSPI_IO1, PIN_QSPI_SCK, PIN_QSPI_IO0),
      _QFlashTransport(PIN_QSPI_CS, _SPI_2), _qFlash(&_QFlashTransport) {}

void FlashLog::begin() {
    // init memory
    if (!_qFlash.begin(&p25q16h, 1)) {
#ifdef DEBUG
        Serial.println("ERROR: failed to initialize QSPI flash chip");
#endif
        return; // TODO: change to return, go to low power, set error status
    }
    _flashOk = true;
    _flashSize = _qFlash.size();
    

#ifdef DEBUG
    Serial.println("STATUS: QSPI flash chip initialized");
#endif

    // test memory
    // check metadata or create

#ifdef DEBUG
    Serial.println("STATUS: no addr in meta. writing");
#endif
    

    // check and load config or create basic

    // confirm end ptr
    findTail();

#ifdef DEBUG
    printInfo();
#endif
}

bool FlashLog::append(uint8_t type, const std::vector<uint8_t> &payload) {
    if (!this->_flashOk) {
#ifdef DEBUG
        Serial.println("ERROR: tried to append. flash is not okay");
#endif
        return false;
    }
    if (_flashSize <= this->_logTailAddr) {
#ifdef DEBUG
        Serial.println("ERROR: tried to append. log is too large");
#endif
        return false;
    }
//     uint32_t waitStart = millis();
//     while (_qFlash.isBusy() && (millis() - waitStart) < 100) {
//         delay(1); // Yield to prevent watchdog resets
//     }
//     if (_qFlash.isBusy()) {
// #ifdef DEBUG
//         Serial.println("ERROR: tried to append. flash timed out");
// #endif
//         return false;
//     }

    // ok. can continue

    uint64_t time = rtc.getTime();
    DataHdr h{type, (uint32_t)payload.size(), time};
#ifdef DEBUG
    Serial.printf("STATUS: writing to address 0x%06x\n", _logTailAddr);
#endif
    _qFlash.writeBuffer(_logTailAddr, (uint8_t *)&h, sizeof(h)); // write header

    // waitStart = millis();
    // while (_qFlash.isBusy() && (millis() - waitStart) < 100) {
    //     delay(1);
    // }


    uint32_t payloadAddr = _logTailAddr + sizeof(h);
#ifdef DEBUG
    Serial.printf("STATUS: writing to address 0x%06lX\n", payloadAddr);
#endif
    _qFlash.writeBuffer(payloadAddr, payload.data(),
                        payload.size()); // write payload

    // update tailAddr
    this->_logTailAddr = payloadAddr + payload.size();

#ifdef DEBUG
    Serial.printf("Status: new tail addr > %06x\n", this->_logTailAddr);
#endif
    return true;
}

bool FlashLog::read(uint32_t addr, DataHdr &header,
                    std::vector<uint8_t> &payload) {
    if (addr >= _logTailAddr) return false;

    // TODO: wait until qflash is ready

    _qFlash.readBuffer(addr, (uint8_t *)&header, sizeof(header));
    uint32_t payloadAddr = addr + sizeof(header);
    payload.resize(header.len);
    _qFlash.readBuffer(payloadAddr, payload.data(), header.len);

    if (header.len != payload.size()) {
#ifdef DEBUG
        Serial.println("ERROR: header length does not match payload size");
#endif
        return false;
    }
#ifdef DEBUG
Serial.printf("STATUS: Reading Addr: %x\n", addr);
    printEntry(header, payload);
#endif

    return true;
}

bool FlashLog::findTail() {
    uint32_t start = millis();
    _qFlash.waitUntilReady();
    uint32_t end = millis();

#ifdef DEBUG
    Serial.printf("STATUS: It took %d for flash to be ready\n", (end - start));
#endif

    uint32_t addr = LOG_OFFSET;
    uint32_t FlashSize = _flashSize * 1024;

    while (addr + sizeof(DataHdr) <= FlashSize) {
        DataHdr h;
        _qFlash.readBuffer(addr, (uint8_t *)&h, sizeof(h));

        if (h.type == 0xFF && h.len == 0xFFFFFFFF) {
#ifdef DEBUG
            Serial.printf("STATUS: Found End Addr > %08X\n", addr);
#endif
            _logTailAddr = addr;
            return true;
        }

        // Prevent infinite loops or wild jumps if data is corrupted
        if (h.len == 0xFFFFFFFF ||
            (addr + sizeof(DataHdr) + h.len > FlashSize)) {
#ifdef DEBUG
            Serial.printf("ERROR: Corrupt log or out of bounds at addr %08X. "
                          "Length: %u\n",
                          addr, h.len);
#endif
            _logTailAddr = addr; // Tail is wherever the corruption started
            return false;
        }

        addr += sizeof(DataHdr) + h.len;
    }
    return false;
}

void FlashLog::cleanConfig() {
    _qFlash.eraseSector(0);
#ifdef DEBUG
    Serial.print("STATUS: waiting to erase config. ");
#endif
    _qFlash.waitUntilReady();
#ifdef DEBUG
    Serial.println("Completed");
#endif
}

void FlashLog::cleanLogs() {
    const uint logSize = _logTailAddr - LOG_OFFSET;
    const uint numOfLogSectors = logSize / SECTOR_SIZE + 16;
    if (numOfLogSectors < 32) {
#ifdef DEBUG
        Serial.printf("STATUS: erasing small log of %u sectors\n",
                      numOfLogSectors - 16);
#endif
        for (uint i = 16; i <= numOfLogSectors; i++)
            _qFlash.erasePage(i);
    } else {
#ifdef DEBUG
        Serial.println("STATUS: erasing large log");
#endif
        for (uint i = 16; i < 32; i++)
            _qFlash.erasePage(i);
        for (uint i = 1; i < 32; i++)
            _qFlash.eraseBlock(i);
    }

#ifdef DEBUG
    Serial.print("STATUS: waiting to erase config. ");
#endif
    _qFlash.waitUntilReady();
#ifdef DEBUG
    Serial.println("Completed");
#endif
}

void FlashLog::cleanAll() {
    _qFlash.eraseChip();
#ifdef DEBUG
    Serial.print("STATUS: waiting to erase chip. ");
#endif
    _qFlash.waitUntilReady();
#ifdef DEBUG
    Serial.println("Completed");
#endif
}

#ifdef DEBUG
void FlashLog::printInfo() {
    Serial.println("---Flash Device Stats--");
    Serial.printf("JEDEC: \t\t0x%X\n", _qFlash.getJEDECID());
    Serial.printf("Flash size: \t%u (KB)\n", (_flashSize / 1024));
    Serial.printf("Page size: \t%u (B)\n", _qFlash.pageSize());
    Serial.printf("Num pages: \t%u\n", _qFlash.numPages());
    Serial.printf("Sector count: \t%u\n", _qFlash.sectorCount());
    Serial.printf("Log Size: \t%u (b)\n", _logTailAddr - LOG_OFFSET);
}

void FlashLog::printMeta() {
    Serial.println("---Address of Log End--");
    Serial.printf("Tail addr: 0x%08x\n", _logTailAddr);
}

void FlashLog::printConfig() {
    ConfigLoad c;
    _qFlash.readBuffer(CFG_OFFSET, (uint8_t *)&c, sizeof(c));

    Serial.println("---Config Data--");
    Serial.printf("PID:\t%u\n", c.pid);

    Serial.println("Threshold Values");
    Serial.printf("\tHigh:\t%u/%u\n", c.systolic_max, c.diastolic_max);
    Serial.printf("\tLow:\t%u/%u\n", c.systolic_min, c.diastolic_min);

    Serial.println("Coefficient Values");
    Serial.printf("\tSystolic:\tS = %u * x + %u\n", c.systolic_coeff_m,
                  c.systolic_coeff_b);
    Serial.printf("\tDiastolic:\tD = %u * x + %u\n", c.diastolic_coeff_m,
                  c.diastolic_coeff_b);
}

void FlashLog::printEntry(const DataHdr &h,
                          const std::vector<uint8_t> &payload) {
    printHeader(h);
    printData(payload);
}

void FlashLog::printHeader(const DataHdr &h) {
    Serial.printf("\tHeader > {\t%d, \t%d,\t%d }\n", h.type, h.len, h.ts);
}

void FlashLog::printData(const std::vector<uint8_t> &payload) {
    Serial.printf("\tPayload > \"");
    Serial.write(payload.data(), payload.size());
    Serial.printf("\"\n");
}

#endif

#ifdef DEBUG_FLASH

void FlashLog::dumpConfig() {
    _qFlash.waitUntilReady();
    Serial.println("---Dump Config Data--");
    this->dump(CFG_OFFSET, PAGE_SIZE); // config is one page
}

void FlashLog::dumpData() {
    Serial.println("---Dump Log Data--");
    this->dump(LOG_OFFSET, _logTailAddr - LOG_OFFSET);
}

void FlashLog::dump(uint32_t start, uint32_t length) {
    const size_t bytesPerLine = 16;
    const uint32_t lines = ((length / 16) + 1) * 16;
    const uint32_t end = start + lines;

    uint8_t buf[bytesPerLine]; // safe for <=32 bytesPerLine

    for (uint32_t addr = start; addr < end; addr += bytesPerLine) {
        uint32_t n = bytesPerLine;
        if (addr + n > end) n = end - addr;

        _qFlash.readBuffer(addr, buf, n);

        // Address
        Serial.printf("%06lX  ", (unsigned long)addr);

        // Hex bytes
        for (uint32_t i = 0; i < bytesPerLine; i++) {
            if (i < n)
                Serial.printf("%02X ", buf[i]);
            else
                Serial.print("   ");
        }

        // ASCII
        Serial.print(" |");
        for (uint32_t i = 0; i < n; i++) {
            char c = (buf[i] >= 32 && buf[i] <= 126) ? (char)buf[i] : '.';
            Serial.print(c);
        }
        Serial.println("|");
    }
}
#endif

FlashLog mem;