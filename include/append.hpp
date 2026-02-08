/**
 * @file append.hpp
 * @brief wrappers to add data to the external flash memory
 * @author Tolu Kolade
 * @date December 28, 2025
 */

#pragma once

#include <cstdint>

/**
 * @brief wrapper to append a debug message to the log section of external
 * memory
 * @param code debug code. the hope is that there is a file for it with defines
 * @return whether the append was successful or nah
 */
bool LOG_DEBUG(const uint8_t code);

/**
 * @brief wrapper to append sensor data to the log section of external
 * memory
 * @return whether the append was successful or nah
 */
bool LOG_SENSOR(const uint8_t heartrate, const uint8_t diastolic,
                const uint8_t systolic, const uint8_t code);