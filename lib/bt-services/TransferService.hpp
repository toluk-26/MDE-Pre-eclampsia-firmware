/**
 * @file TransferService.hpp
 * @author Tolu Kolade
 * @brief Transfer Service. used to stream packets in flash data block
 * @date March 29, 2026
 */

#include <BLECharacteristic.h>
#include <BLEService.h>
#include <vector>

#define UUID_TRANSFER_SERVICE "8d760000-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_TRIGGER "8d760001-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_DATA "8d760002-7bdb-4430-a1b9-e7d26fb2b981"
#define UUID_CHR_DATA_SIZE "8d760003-7bdb-4430-a1b9-e7d26fb2b981"

class TransferService : public BLEService {
  public:
    /// @brief sets Service UUID
    TransferService() : BLEService(UUID_TRANSFER_SERVICE) {};
    /// @brief sets characteristics properties
    virtual err_t begin();

    /// @brief must be checked in ble state. if true, set to false and begin
    /// transfering data from flash to data service
    bool transfer_flag = false;

    /// @brief send a packet of data. 
    void sendData(const std::vector<uint8_t> &data);

    /// @date send the number of packets sent to confirm
    void sendSize(const uint32_t &size);

  protected:
    BLECharacteristic _trigger; // trigger transfer
    BLECharacteristic _data;    // transfers data
    BLECharacteristic _size;    // size of data

    static void trigger_cb(uint16_t conn_hdl, BLECharacteristic *chr,
                           uint8_t *data, uint16_t len);
    static void start(uint16_t conn_hdl, BLECharacteristic *chr,
                      uint16_t value);
};