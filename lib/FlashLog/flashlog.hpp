/**
 * @file flashlog.hpp
 * @brief class definition to manipulate the external flash memory
 * @author Tolu Kolade
 * @date December 27, 2025
 */

#pragma once

#include <Adafruit_SPIFlash.h>
#include <SPI.h>
#include <Wire.h>
#include <cstdint>
#include <vector>

#define LOG_O 0x001000

// TODO: move to a better place
#pragma pack(push, 1)
struct ConfigPack {
    uint32_t pid; // patient id TODO: confirm if size is okay
    uint8_t diastolic_min;
    uint8_t diastolic_max;
    float diastolic_coeff_m; // TODO: confirm data type
    float diastolic_coeff_b; // TODO: confirm data type
    uint8_t systolic_min;
    uint8_t systolic_max;
    float systolic_coeff_m; // TODO: confirm data type
    float systolic_coeff_b; // TODO: confirm data type
};
#pragma pack(pop)

#pragma pack(push, 1)
struct DataHdr {
    uint8_t type; // PayloadType
    uint32_t len; // payload length
    uint64_t ts;  // unix seconds
};
#pragma pack(pop)

// QSPI flash device information. from online
SPIFlash_Device_t const p25q16h{
    .total_size = (1UL << 21), // 2MiB
    .start_up_time_us = 10000,
    .manufacturer_id = 0x85,
    .memory_type = 0x60,
    .capacity = 0x15,
    .max_clock_speed_mhz = 55,
    .quad_enable_bit_mask = 0x02,
    .has_sector_protection = 1,
    .supports_fast_read = 1,
    .supports_qspi = 1,
    .supports_qspi_writes = 1,
    .write_status_register_split = 1,
    .single_status_byte = 0,
    .is_fram = 0,
};

class FlashLog {
  public:
    enum PayloadType : uint8_t { SENSOR_t, DEBUG_t };

    /// @brief creates needed external variables
    FlashLog();

    /// @brief initializes qflash and checks config
    void begin();

    /**
     * @brief forms a packets by type and puts it at the end of the data section
     * of qflash memory
     * @param type of PayloadType
     * @param payload preformatted vector of data
     * @return whether it appended or nah
     */
    bool append(PayloadType type, const std::vector<uint8_t> &payload);

    /**
     * @param addr address to read packet from
     * @param header saves header to this
     * @param payload saves payload to this
     * @return successful read
     */
    bool read(uint32_t addr, DataHdr &header, std::vector<uint8_t> &payload);

    ConfigPack getConfig();
    uint8_t getTz();
    bool setConfig();

    /// @brief deletes config section
    void cleanConfig(); 
    /// @brief deletes data section
    void cleanLogs();
    /// @brief deletes entire chip
    void cleanAll();

#ifdef DEBUG // debug functions
    void printInfo();
    void printMeta();
    void printConfig();
    void printEntry(const DataHdr &h, const std::vector<uint8_t> &payload);
    void printHeader(const DataHdr &h);
    void printData(const std::vector<uint8_t> &payload);
    void dumpConfig();
    void dumpData();
    void dump() { dump(0, _flashSize); };
    void dump(uint32_t start, uint32_t length);
#endif

  protected:
    const uint32_t PAGE_SIZE = 0x000100;   // 256 bytes
    const uint32_t SECTOR_SIZE = 0x001000; // 4 KB
    const uint32_t BLOCK_SIZE = 0x010000;  // 64 KB
    const uint32_t CFG_OFFSET = 0x000000;  // config data block
    const uint32_t LOG_OFFSET = LOG_O;     // log block

    ConfigPack configload;

    SPIClass _SPI_2;
    Adafruit_FlashTransport_SPI _QFlashTransport;
    Adafruit_SPIFlash _qFlash;

  private:
    bool _flashOk = false;              // is the QSPI okay?
    uint32_t _flashSize = 0;            // flash size 2Mb
    uint32_t _logTailAddr = LOG_OFFSET; // size of the logs

    /**
     * @brief finds where the data ends and sets the tail addr var to it. runs
     * once at start.
     * @return found tail addr or nah
     */
    bool findTail();
};

extern FlashLog mem;