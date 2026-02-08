/**
 * @file append.cpp
 * @brief wrappers to add data to the external flash memory
 * @author Tolu Kolade
 * @date December 28, 2025
 */

#include "append.hpp"
#include "flashlog.hpp"

bool LOG_DEBUG(const uint8_t code) {
    return mem.append(FlashLog::RecType::DEBUG_t, {code});
}

bool LOG_SENSOR(const uint8_t heartrate, const uint8_t diastolic,
                const uint8_t systolic, const uint8_t code) {
    return mem.append(FlashLog::RecType::SENSOR_t,
                      {heartrate, diastolic, systolic, code});
}
