#include "flashlog.hpp"

bool LOG_DEBUG(const uint8_t code) {
    return mem.append(FlashLog::RecType::DEBUG_t, {code});
}

bool LOG_SENSOR(const uint8_t heartrate, const uint8_t diastolic,
                const uint8_t systolic, const uint8_t code) {
    return mem.append(FlashLog::RecType::SENSOR_t,
                      {heartrate, diastolic, systolic, code});
}
