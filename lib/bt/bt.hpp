/**
 * @file bt.hpp
 * @author Tolu Kolade
 * @brief ble device functions and starts services
 * @date December 23, 2025
 */

#pragma once
#include <bluefruit.h>

class PESBt {
  public:
    /// @brief initialize ble and services
    void init();

    /// @brief turn off ble
    void disconnect();

  protected:
    /// @brief what to do on disconnect from phone
    static void disconnect_callback(uint16_t conn_handle, uint8_t reason);

    /// @brief what to do on connect to phone
    static void connect_callback(uint16_t conn_handle);

    /// @brief uhh start advertising
    static void startAdv();

    /// @brief ble device info service
    void dev_info_config();

    /**
     * @brief configure the ble device for broadcasting
     */
    void ble_config();
};