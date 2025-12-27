#pragma once
#include <bluefruit.h>

class PESBt {
  public:
    /// @brief initialize ble and services
    void init();

    /// @brief turn off ble
    void disconnect();

  protected:
    static void disconnect_callback(uint16_t conn_handle, uint8_t reason);
    static void connect_callback(uint16_t conn_handle);
    static void startAdv();

    /// @brief ble device info service
    void dev_info_config();

    /**
     * @brief configure the ble device for broadcasting
     */
    void ble_config();
};