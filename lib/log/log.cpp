/**
 * @file log.cpp
 * @brief wrappers to add data to the external flash memory
 * @author Tolu Kolade
 * @date December 28, 2025
 */

#include "log.hpp"
#include "flashlog.hpp"

bool Log::log(LogLevel level, const char *file, const char *func, int line,
              const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool res = vlog(level, file, func, line, fmt, args);
    va_end(args);
    return res;
}

bool Log::sensor(const uint8_t heartrate, const uint8_t diastolic,
                 const uint8_t systolic, const uint8_t code) {
    LOGV("Saving Sensor values %d, %d, %d, %d", heartrate, diastolic, systolic,
         code);
    return mem.append(FlashLog::RecType::SENSOR_t,
                      {heartrate, diastolic, systolic, code});
}

bool Log::vlog(LogLevel level, const char *file, const char *func, int line,
               const char *fmt, va_list args) {
    // create input string with args
    char body[128];
    vsnprintf(body, sizeof(body), fmt, args);

    // format file string
    String f = file;
    f = f.substring(f.lastIndexOf('/') + 1);
    f = f.substring(f.lastIndexOf('\\') + 1);

    String msg;
    // write to serial monitor
#ifdef DEBUG
    msg = "[";
    msg += lvltoString(level);
    msg += "] ";

    if (file && func) {
        msg += f;
        msg += ":";
        msg += func;
        msg += "():";
        msg += line;
        msg += " > ";
    }
    msg += body;
    Serial.println(msg);
#endif

// write to memory
    if (level != LogLevel::VERBOSE) {
        // new format
        msg = String((int)level);
        msg += f;
        msg += ':';
        msg += func;
        msg += "() > ";
        msg += body;

        std::vector<uint8_t> data(msg.length() + 1);
        msg.getBytes(data.data(), msg.length() + 1);
        return mem.append(FlashLog::RecType::DEBUG_t, data);
    }

    return true;
}

const String Log::lvltoString(LogLevel level) {
    switch (level) {
    case LogLevel::ERROR:
        return "E";
    case LogLevel::WARN:
        return "W";
    case LogLevel::STATUS:
        return "S";
    case LogLevel::VERBOSE:
        return "V";
    default:
        return "?";
    }
}