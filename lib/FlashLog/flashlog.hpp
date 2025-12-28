#pragma once

#include <Adafruit_SPIFlash.h>
#include <SPI.h>
#include <Wire.h>
#include <cstdint>
#include <vector>

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

bool LOG_DEBUG(const uint8_t code);
bool LOG_SENSOR(const uint8_t heartrate, const uint8_t diastolic,
                const uint8_t systolic, const uint8_t code);

class FlashLog {
  public:
    /**
     * @brief check the external flash memory and confirm data or create
     * template
     */
    FlashLog();
    bool _findTail(); // TODO: move to private

    /**
     * @brief append data to data block
     */
    bool append(uint8_t type, const std::vector<uint8_t> &payload);

    bool getConfig();
    bool getData();

    bool setConfig();

    void cleanConfig();
    void cleanLogs();
    void cleanAll();

    enum RecType : uint8_t { SENSOR_t, DEBUG_t };

#ifdef DEBUG // debug functions
    void printInfo();
    void printMeta();
    void printConfig();
    void printData();
#endif
#ifdef DEBUG_FLASH
    void dumpConfig();
    void dumpData();
    void dump();
    void dump(uint32_t start, uint32_t length);
#endif

  protected:
    const uint32_t BLOCK_SIZE = 0x001000; // 4KB sector

    const uint32_t CFG_OFFSET = 0x000000; // config data block
    const uint32_t LOG_OFFSET = 0x001000; // log block

#pragma pack(push, 2)
    struct DataHdr {
        uint8_t type; // RecType
        uint16_t len; // payload length
        uint32_t ts;  // unix seconds
    };
#pragma pack(pop)

    struct ConfigLoad {
        uint32_t pid; // patient id TODO: confirm if size is okay
        uint32_t thres_dia;
        uint32_t thres_sys;
    };

    SPIClass SPI_2;
    Adafruit_FlashTransport_SPI QFlashTransport;
    Adafruit_SPIFlash QFlash;

  private:
    bool _flashOk = false;              // is the QSPI okay?
    uint32_t _flashSize = 0;            // flash size 2Mb
    uint32_t _logTailAddr = LOG_OFFSET; // size of the logs
};

extern FlashLog mem;