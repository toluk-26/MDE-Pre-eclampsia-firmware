#include "battery.h"
#include <nrf.h>
#include <nrfx_power.h>

// Event flag shared between ISR and main
static volatile bool usb_event_flag = false;

// Cached state (optional but nice)
static volatile bool usb_present_cached = false;

// Direct hardware read
static inline bool usb_vbus_present() {
    return (NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk) != 0;
}

// Interrupt callback (ISR context)
static void on_usb_evt(nrfx_power_usb_evt_t evt)
{
    switch (evt) {
        case NRFX_POWER_USB_EVT_DETECTED:
        case NRFX_POWER_USB_EVT_REMOVED:
        case NRFX_POWER_USB_EVT_READY:
            usb_present_cached = usb_vbus_present();
            usb_event_flag = true;
            break;
        default:
            break;
    }
}

// Initialize power detection system
void power_init()
{
    nrfx_power_usbevt_config_t cfg = {};
    cfg.handler = on_usb_evt;

    nrfx_power_usbevt_init(&cfg);
    nrfx_power_usbevt_enable();

    // Initialize state
    usb_present_cached = usb_vbus_present();
    usb_event_flag = true;
}

// Get current power state
PowerState power_get_state()
{
    return usb_present_cached ? POWER_USB : POWER_BATTERY;
}

// Check if event occurred
bool power_event_available()
{
    return usb_event_flag;
}

// Clear event flag
void power_clear_event()
{
    usb_event_flag = false;
}