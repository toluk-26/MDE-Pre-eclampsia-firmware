/**
 * @file bt.hpp
 * @author Tolu Kolade
 * @brief ble device functions and starts services
 * @date December 23, 2025
 */

#pragma once
#include <bluefruit.h>

#include "CalibrateService.hpp"
#include "ConfigService.hpp"
#include "TimeService.hpp"
#include "TransferService.hpp"

class BleManager {
  public:
    /// @brief initialize ble and services
    void begin();

    /// @brief turn off ble
    void stop();

    /// @brief needs to be public for the controller
    TransferService transferService;
    CalibrateService calibrateService;
    ConfigService configService; // handles configuration settings

    static bool disconnectFlag;

  private:
    /// @brief what to do on disconnect from phone
    static void onDisconnect(uint16_t conn_handle, uint8_t reason);

    /// @brief what to do on connect to phone
    static void onConnect(uint16_t conn_handle);

    /// @brief uhh start advertising
    static void startAdvertising();

    /// @brief ble device info service
    void configDeviceInfo();

    /// @brief configure the ble device for broadcasting
    void configBleHardware();

    // Bluefruit Services
    BLEDfu _dfu;     // OTA DFU service
    BLEDis _devInfo; // device information
    BLEBas _battery; // battery info

    // PES services
    TimeService timeService;     // update time and tz
    
};

extern BleManager bt;