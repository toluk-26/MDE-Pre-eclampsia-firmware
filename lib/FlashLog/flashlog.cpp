/**
 * @file flashlog.cpp
 * @author Tolu Kolade
 * @date December 27, 2025
 *
 * @todo missing function implementations
 * @todo split header guarded blocks into separate .cpp files
 * @todo setTz()
 */

#include "flashlog.hpp"
#include "bt.hpp"
#include "log.hpp"
#include "rtc.hpp"

FlashLog::FlashLog()
    : _SPI_2(NRF_SPIM0, PIN_QSPI_IO1, PIN_QSPI_SCK, PIN_QSPI_IO0),
      _QFlashTransport(PIN_QSPI_CS, _SPI_2), _qFlash(&_QFlashTransport) {}

void FlashLog::begin() {
    if (_flashOk) return; // check if flash is already init

    // init memory
    if (!_qFlash.begin(&p25q16h, 1)) {
        _flashOk = false;
        LOGV("failed to initialize QSPI flash chip"); // no point in error since
                                                      // flash isn't working
                                                      // anyways

        return; // TODO: change to return, go to low power, set error status
    }
    _flashOk = true;
    _flashSize = _qFlash.size();

    LOGV("QSPI flash chip initialized");

    // test memory
    // check metadata or create
    LOGV("no addr in meta. writing");

    // TODO: check and load config or create basic

    // confirm end ptr
    findTail();

#ifdef DEBUG
    printInfo();
#endif
}

bool FlashLog::append(PayloadType type, const std::vector<uint8_t> &payload) {
    if (!this->_flashOk) {
        LOGE("tried to append. flash is not okay");
        return false;
    }
    if (_flashSize <= this->_logTailAddr) {
        LOGE("tried to append. log is too large");
        return false;
    }
    // TODO: try not to write too quickly
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
    // prepare header
    uint64_t time = rtc.getTime();
    DataHdr h{type, payload.size(), time};
    LOGV("writing header to address 0x%06x", _logTailAddr);
#ifndef DEBUG_FLASH
    _qFlash.writeBuffer(_logTailAddr, (uint8_t *)&h, sizeof(h)); // write header
#endif

    // waitStart = millis();
    // while (_qFlash.isBusy() && (millis() - waitStart) < 100) {
    //     delay(1);
    // }

    // prepare payload
    uint32_t payloadAddr = _logTailAddr + sizeof(h);
    LOGV("writing payload to address 0x%06lX", payloadAddr);
#ifndef DEBUG_FLASH
    _qFlash.writeBuffer(payloadAddr, payload.data(),
                        payload.size()); // write payload
#endif

    // update tailAddr
    this->_logTailAddr = payloadAddr + payload.size();

    LOGV("New tail addr > %06x", this->_logTailAddr);
    return true;
}

bool FlashLog::read(uint32_t addr, DataHdr &header,
                    std::vector<uint8_t> &payload) {
    if (addr >= _logTailAddr) return false; // TODO: log?

    // TODO: wait until qflash is ready

    _qFlash.readBuffer(addr, (uint8_t *)&header, sizeof(header)); // read header
    uint32_t payloadAddr = addr + sizeof(header); // start addr of payload
    payload.resize(header.len);                   // prepare return payload size
    _qFlash.readBuffer(payloadAddr, payload.data(), header.len); // get payload

    // this seems dumb
    // if (header.len != payload.size()) {
    //     LOGE("header length does not match payload size");
    //     return false;
    // }

    // #ifdef DEBUG
    //     LOGV("Reading Addr: %x", addr);
    //     printEntry(header, payload);
    // #endif
    return true;
}

ConfigPack FlashLog::getConfig() {
    // this is here repeated because bt starts before config and tries to use
    // the memory too fast.
    if (!this->_flashOk) {
        LOGV3("Flash is not okay. trying to begin");
        this->begin();
        if (!this->_flashOk) {
            LOGV3("Flash is really not okay");
            return ConfigPack();
        }
    }
    // get config
    ConfigPack c;
    _qFlash.readBuffer(0, (uint8_t *)&c, sizeof(configload));

    // check config load from flash
    // TODO: if null, write default values.

#ifdef DEBUG
    printConfig();
#endif

    return c;
}

uint8_t FlashLog::getTz() {
    if (!this->_flashOk) {
        LOGV3("Flash is not okay");
        return 0;
    }
    uint8_t tz;
    _qFlash.readBuffer(sizeof(configload), (uint8_t *)&tz, sizeof(tz));
    return tz;
}

bool FlashLog::findTail() {
    uint32_t addr = LOG_OFFSET;
    uint32_t FlashSize = _flashSize * 1024;

    while (addr + sizeof(DataHdr) <= FlashSize) {
        DataHdr h;
        _qFlash.readBuffer(addr, (uint8_t *)&h, sizeof(h));

        // check if header values are null
        if (h.type == 0xFF && h.len == 0xFFFFFFFF) {
            LOGV("Found End Addr > %08X", addr);
            _logTailAddr = addr;
            return true;
        }

        // Prevent infinite loops or wild jumps if data is corrupted
        if (h.len == 0xFFFFFFFF ||
            (addr + sizeof(DataHdr) + h.len > FlashSize)) {
            LOGE("Corrupt log or out of bounds at addr %08X. Length: %u", addr,
                 h.len);
            _logTailAddr = addr; // Tail is wherever the corruption started
            return false;
        }

        // increment
        addr += sizeof(DataHdr) + h.len;
    }
    return false;
}

bool FlashLog::setConfig() {
    if (!this->_flashOk) {
        LOGE("tried to write config. flash is not okay");
        return false;
    }

    LOGV("Writing to address 0x%06x", 0);
#ifndef DEBUG_FLASH
    _qFlash.writeBuffer(0, (uint8_t *)&configload,
                        sizeof(configload)); // write header
#endif

    // LOGV("updating ble");
    // bt.configService.updateConfig(configload);

    return true;
}

void FlashLog::cleanConfig() {
    _qFlash.eraseSector(0);
    LOGV("waiting to erase config.");
    _qFlash.waitUntilReady();
    LOGV("Completed");
}

void FlashLog::cleanLogs() {
    const uint logSize = _logTailAddr - LOG_OFFSET;
    const uint numOfLogSectors = logSize / SECTOR_SIZE + 16;
    if (numOfLogSectors < 32) {
        LOGV("Erasing small log of %u sectors", numOfLogSectors - 16);
        for (uint i = 16; i <= numOfLogSectors; i++)
            _qFlash.erasePage(i);
    } else {
        LOGV("erasing large log");
        for (uint i = 16; i < 32; i++)
            _qFlash.erasePage(i);
        for (uint i = 1; i < 32; i++)
            _qFlash.eraseBlock(i);
    }

    LOGV3("Waiting to erase Logs.");
    _qFlash.waitUntilReady();
    LOGV3("Completed");
    findTail();
}

void FlashLog::cleanAll() {
    _qFlash.eraseChip();
    LOGV3("Waiting to erase chip.");
    _qFlash.waitUntilReady();
    LOGV3("Completed.");
    findTail();
}

#ifdef DEBUG
void FlashLog::printInfo() {
    LOGV("---Flash Device Stats--");
    LOGV("JEDEC: \t\t0x%X", _qFlash.getJEDECID());
    LOGV("Flash size: \t%u (KB)", (_flashSize / 1024));
    LOGV("Page size: \t%u (B)", _qFlash.pageSize());
    LOGV("Num pages: \t%u", _qFlash.numPages());
    LOGV("Sector count: \t%u", _qFlash.sectorCount());
    LOGV("Log Size: \t%u (b)", _logTailAddr - LOG_OFFSET);
}

void FlashLog::printMeta() {
    LOGV("---Address of Log End--");
    LOGV("Tail addr: 0x%08x", _logTailAddr);
}

void FlashLog::printConfig() {
    ConfigPack c;
    _qFlash.readBuffer(CFG_OFFSET, (uint8_t *)&c, sizeof(c));

    LOGV("---Config Data--");
    LOGV("PID:\t%u", c.pid);

    LOGV("Threshold Values");
    LOGV("\tHigh:\t%u/%u", c.systolic_max, c.diastolic_max);
    LOGV("\tLow:\t%u/%u", c.systolic_min, c.diastolic_min);

    LOGV("Coefficient Values");
    LOGV("\tSystolic:\tS = %f * x + %f", c.systolic_coeff_m,
         c.systolic_coeff_b);
    LOGV("\tDiastolic:\tD = %f * x + %f", c.diastolic_coeff_m,
         c.diastolic_coeff_b);
}

void FlashLog::printEntry(const DataHdr &h,
                          const std::vector<uint8_t> &payload) {
    printHeader(h);
    printData(payload);
}

void FlashLog::printHeader(const DataHdr &h) {
    LOGV("\tHeader > {\t%d, \t%d,\t%u%u }", h.type, h.len,
         static_cast<uint32_t>(h.ts >> 32), static_cast<uint32_t>(h.ts));
}

void FlashLog::printData(const std::vector<uint8_t> &payload) {
    LOGV("\tPayload > \"%.*s\"", payload.size(), payload.data());
}

void FlashLog::dumpConfig() {
    _qFlash.waitUntilReady();
    LOGV("---Dump Config Data--");
    this->dump(CFG_OFFSET, PAGE_SIZE); // config is one page
}

void FlashLog::dumpData() {
    LOGV("---Dump Log Data--");
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