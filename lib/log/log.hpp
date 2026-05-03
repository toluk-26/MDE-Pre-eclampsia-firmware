/**
 * @file log.hpp
 * @brief wrappers to add data to the external flash memory. log class and
 * macros
 * @author Tolu Kolade
 * @date December 28, 2025
 */

#pragma once

#include <Arduino.h>
#include <cstdint>

/**
 * @brief log an error to serial monitor and flash. input in the same way you
 * would println() or printf. the new line character is already added for you.
 * ex: LOGE("Something bad happened!"), LOGE("Something bad happened! %d",
 * value)
 *
 *
 * @param fmt a string like "something bad happened"
 * @param __VA_ARGS__... arguments for values like example 2
 */
#define LOGE(fmt, ...)                                                         \
    Log::log(LogLevel::ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * @brief log an warning to serial monitor and flash. input in the same way you
 * would println() or printf. the new line character is already added for you.
 * ex: LOGE("Something odd happened!"), LOGE("Something odd happened! %d",
 * value)
 *
 * @param fmt a string like "something odd happened"
 * @param __VA_ARGS__... arguments for values like example 2
 */
#define LOGW(fmt, ...)                                                         \
    Log::log(LogLevel::WARNING, __FILE__, __func__, __LINE__, fmt,             \
             ##__VA_ARGS__)

/**
 * @brief log something to serial monitor and flash. input in the same way you
 * would println() or printf. the new line character is already added for you.
 * ex: LOGE("Something happened!"), LOGE("Something happened! %d",
 * value)
 *
 * @param fmt a string like "something happened"
 * @param __VA_ARGS__... arguments for values like example 2
 */
#define LOGS(fmt, ...)                                                         \
    Log::log(LogLevel::STATUS, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef DEBUG
/**
 * @brief verbose log something to serial monitor and but NOT flash. input in
 * the same way you would println() or printf. the new line character is already
 * added for you. ex: LOGE("Something happened!"), LOGE("Something happened!
 * %d", value). this does not print file:function:line
 *
 * @param fmt a string like "something happened"
 * @param __VA_ARGS__... arguments for values like example 2
 */
#define LOGV(fmt, ...)                                                         \
    Log::log(LogLevel::VERBOSE, nullptr, nullptr, 0, fmt, ##__VA_ARGS__)

/**
 * @brief level 3 verbose log something to serial monitor and but NOT flash.
 * input in the same way you would println() or printf. the new line character
 * is already added for you. ex: LOGE("Something happened!"), LOGE("Something
 * happened! %d", value). this versions says the whole file, function and line
 * in output
 *
 * @param fmt a string like "something happened"
 * @param __VA_ARGS__... arguments for values like example 2
 */
#define LOGV3(fmt, ...)                                                        \
    Log::log(LogLevel::VERBOSE, __FILE__, __func__, __LINE__, fmt,             \
             ##__VA_ARGS__)
#else
#define LOGV(fmt, ...)
#define LOGV2(fmt, ...)
#endif

enum LogLevel : uint8_t { ERROR, WARN, STATUS, VERBOSE };

class Log {
  public:
    /// @brief save a debug log
    static bool log(LogLevel level, const char *file, const char *func,
                    int line, const char *fmt, ...);

    /**
     * @brief wrapper to append sensor data to the log section of external
     * memory
     * @return whether the append was successful or nah
     */
    static bool sensor(const uint8_t heartrate, const uint8_t systolic,
                       const uint8_t diastolic, const uint8_t code);

  private:
    static bool vlog(LogLevel level, const char *file, const char *func,
                     int line, const char *fmt, va_list args);
    static const String lvltoString(LogLevel level);
};