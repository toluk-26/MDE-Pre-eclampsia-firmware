#include <Arduino.h>
#include <nrf.h> // included to use code to access hardware registers to detect when usb is connected vs not

// Returns true if USB 5V (VBUS) is physically present on the connector.
// This uses the nRF52840 POWER peripheral hardware register.
//
// The USBREGSTATUS register contains status bits related to USB power.
// POWER_USBREGSTATUS_VBUSDETECT_Msk is the bitmask for the VBUS detect flag.
//
// If VBUS (5V from USB) is present, the bit is 1.
// If USB is unplugged, the bit is 0.

static inline bool usb_vbus_present() {
  return (NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk) != 0;
}

// Active-low LED helpers (common anode)
static inline void led_on(uint8_t pin)  { digitalWrite(pin, LOW); }
static inline void led_off(uint8_t pin) { digitalWrite(pin, HIGH); }

static inline void rgb(bool red, bool green, bool blue) {
  // Set modes once
  static bool init = false;
  if (!init) {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    init = true;
  }

  red   ? led_on(LED_RED)   : led_off(LED_RED);
  green ? led_on(LED_GREEN) : led_off(LED_GREEN);
  blue  ? led_on(LED_BLUE)  : led_off(LED_BLUE);
}

void setup() {
  rgb(false, false, false); // all off
}

void loop() {
  if (usb_vbus_present()) {
    // USB present: GREEN
    rgb(false, true, false);
  } else {
    // Battery only: RED
    rgb(true, false, false);
  }
  delay(200);
}