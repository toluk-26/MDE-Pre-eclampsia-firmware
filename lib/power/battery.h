#pragma once

#include <stdbool.h>

// Power state enum (clean for FSM use)
typedef enum {
    POWER_USB,        // USB connected (charging)
    POWER_BATTERY     // Battery only
} PowerState;

// Initialize power/USB detection system
void power_init();

// Returns current power state (can be called anytime)
PowerState power_get_state();

// Returns true if a power event occurred (USB plug/unplug)
bool power_event_available();

// Clears the event flag (after FSM consumes it)
void power_clear_event();