#include <Arduino.h>
#include <nrf.h>            // Direct register access (NRF_POWER, etc.)
#include <nrfx_power.h>     // Nordic driver layer for power/USB events

static inline void led_on(uint8_t pin)  { digitalWrite(pin, LOW); }
static inline void led_off(uint8_t pin) { digitalWrite(pin, HIGH); }

static inline void rgb(bool r, bool g, bool b) {
  led_off(LED_RED);   led_off(LED_GREEN);   led_off(LED_BLUE);
  if (r) led_on(LED_RED);
  if (g) led_on(LED_GREEN);
  if (b) led_on(LED_BLUE);
}

/*
   This reads the hardware USB power detection register.

   NRF_POWER->USBREGSTATUS contains status bits.
   POWER_USBREGSTATUS_VBUSDETECT_Msk is the mask
   for the "USB cable is present" bit.

   This function works anytime - not dependent on interupts.
*/
static inline bool usb_vbus_present() {
  return (NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk) != 0;
}

/*
   volatile is REQUIRED because:
   - This variable is modified inside an ISR (interrupt context)
   - It is read inside the main loop (normal execution context)
   Without volatile, compiler optimizations could break it.
*/

static volatile bool usb_event_flag = false;

static void on_usb_evt(nrfx_power_usb_evt_t evt)
{
  switch (evt) {
    // USB cable detected (VBUS applied)
    case NRFX_POWER_USB_EVT_DETECTED:
    // USB cable removed
    case NRFX_POWER_USB_EVT_REMOVED:
    // USB regulator ready
    case NRFX_POWER_USB_EVT_READY:
      // Signaling main loop to update state
      usb_event_flag = true;
      break;
    default:
      break;
  }
}

void setup() {
  // Configuring RGB pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Start from known OFF state (active-low)
  rgb(false, false, false);

  /*
    Configure USB event subsystem.

    nrfx_power_usbevt_config_t holds:
       - handler pointer

    In this version of nrfx:
       - INIT registers the handler
       - ENABLE activates interrupts
  */
  nrfx_power_usbevt_config_t cfg = {};
  cfg.handler = on_usb_evt;

  nrfx_power_usbevt_init(&cfg);
  nrfx_power_usbevt_enable();   // <-- no args -> is what actually wakes the MCU up

  // Force initial update so LED shows current power source immediately
  usb_event_flag = true;
}

void loop() {
  /*
     1. Interrupt sets usb_event_flag
     2. Loop sees flag
     3. Loop handles state change
     4. CPU goes back to sleep
  */

  // Only react if interrupt signaled a change
  if (usb_event_flag) {
    usb_event_flag = false; // Clear event

    bool usb = usb_vbus_present();

    // USB present -> BLUE, Battery only -> RED
    if (usb) rgb(false, false, true);
    else     rgb(true, false, false);
  }

  // Sleep until an interrupt wakes us (USB events should wake us)
  __WFE(); // WFE = Wait for Event (ARM Cortex-M instruction) - clears any old event
  __SEV(); // SEV = Send Event (ARM Cortex-M instruction) - sets fresh event
  __WFE(); // - waits properly so CPU does wake up/goes to sleep
}
