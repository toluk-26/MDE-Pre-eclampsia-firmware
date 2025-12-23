#pragma once
#include <Arduino.h>
#include <bluefruit.h>

void disconnect_callback(uint16_t conn_handle, uint8_t reason);
void connect_callback(uint16_t conn_handle);
void startAdv(void);

extern BLEUart bleuart; // uart over ble

class PES_BLE {
  public:
    enum service {};

    void init();
    bool write(String str);
    bool available();
    void disconnect();

  private:
};