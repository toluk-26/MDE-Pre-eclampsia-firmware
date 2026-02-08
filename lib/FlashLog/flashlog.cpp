/**
 * @file flashlog.cpp
 * @author Tolu Kolade
 * @date December 27, 2025
 * @todo missing function implementations
 * @todo split header guarded blocks into separate .cpp files
 */

#include "flashlog.hpp"

FlashLog::FlashLog()
    : _SPI_2(NRF_SPIM0, PIN_QSPI_IO1, PIN_QSPI_SCK, PIN_QSPI_IO0),
      _QFlashTransport(PIN_QSPI_CS, _SPI_2), _qFlash(&_QFlashTransport) {

    // init memory
    if (!_qFlash.begin(&p25q16h, 1)) {
#ifdef DEBUG
        Serial.println("ERROR: failed to initialize QSPI flash chip");
#endif
        return; // TODO: change to return, go to low power, set error status
    }
#ifdef DEBUG
    Serial.println("STATUS: QSPI flash chip initialized");
    printInfo();
#endif

    _flashOk = true;
    _flashSize = _qFlash.size();

    // test memory
    // check metadata or create

#ifdef DEBUG
    Serial.println("STATUS: no addr in meta. writing");
#endif
    this->_findTail();

    // check and load config or create basic

    // confirm end ptr
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
    while (_qFlash.isBusy() && millis() < 100) {
        ;
    }
    //     if (QFlash.isBusy()) {
    // #ifdef DEBUG
    //         Serial.println("ERROR: tried to append. flash timed out");
    // #endif
    //         return false;
    //     }

    // ok. can continue

    uint32_t time = this->_logTailAddr; // TODO: get from rtc
    DataHdr h{type, (uint16_t)payload.size(), time};
#ifdef DEBUG
    Serial.printf("STATUS: writing to address 0x%06x\n", _logTailAddr);
#endif
    _qFlash.writeBuffer(_logTailAddr, (uint8_t *)&h, sizeof(h)); // write header
    uint32_t payloadAddr = _logTailAddr + sizeof(h);
#ifdef DEBUG
    Serial.printf("STATUS: writing to address 0x%06lX\n", payloadAddr);
#endif
    _qFlash.writeBuffer(payloadAddr, (uint8_t *)payload.data(),
                        payload.size()); // write payload

    // update tailAddr
    this->_logTailAddr = payloadAddr + payload.size();

#ifdef DEBUG
    Serial.printf("Status: new tail addr > %06x\n", this->_logTailAddr);
#endif
    return true;
}

bool FlashLog::_findTail() {
    uint32_t addr = LOG_OFFSET;

    while (addr + sizeof(DataHdr) <= _flashSize) {
        DataHdr h;
        _qFlash.readBuffer(addr, (uint8_t *)&h, sizeof(h));

        // check if header is empty
        const uint8_t *p = (const uint8_t *)&h;
        for (size_t i = 0; i < sizeof(DataHdr); i++) {
            if (p[i] == 0xFF) {
#ifdef DEBUG
                Serial.printf("STATUS: Found End Addr > %08X\n", addr);
#endif
                this->_logTailAddr = addr;
                return true;
            }
        }

        // addr isnt a tail. continue to the next block
#ifdef DEBUG_FLASH
        Serial.printf("STATUS: Header {\t%d, \t%d,\t%d }\n", h.type, h.len,
                      h.ts);
#endif

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
    if (numOfLogSectors < 32){
#ifdef DEBUG
        Serial.printf("STATUS: erasing small log of %u sectors\n", numOfLogSectors - 16);
#endif
        for (uint i = 16; i <= numOfLogSectors; i++)
            _qFlash.erasePage(i);}
    else {
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
    Serial.printf("\tDiastolic:\t%u\n", c.thres_dia);
    Serial.printf("\tSystolic:\t%u\n", c.thres_sys);
}

void FlashLog::printData() {}

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