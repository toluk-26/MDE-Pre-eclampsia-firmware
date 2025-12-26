#pragma once
#include <Arduino.h>
#include <bluefruit.h>

void disconnect_callback(uint16_t conn_handle, uint8_t reason);
void connect_callback(uint16_t conn_handle);
void startAdv(void);

// TODO: move
class PES_BLE {
  public:

    void init();
    void disconnect();

  private:
};