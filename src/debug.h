#pragma once

#include <Arduino.h>
#include <TimeLib.h>

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 115200
#endif

#define LOG_LEVEL_DEBUG "DEBUG"
#define LOG_LEVEL_INFO "INFO "
#define LOG_LEVEL_WARN "WARN "
#define LOG_LEVEL_ERROR "ERROR"
#define LOG_LEVEL_CRIT "CRIT "

#ifdef CONSOLE
class Console
{
public:
    Console(bool waitForConnection = false);
    ~Console();
    size_t log(const char *logLevel, const char *format, ...);
    void hexDump(const char *logLevel, unsigned char *buf, size_t size);

private:
    bool isContinuosMessage = false;

    size_t vprintf(const char *format, va_list arg);
};

extern Console console;

#ifdef DEBUG
#define LOG_DEBUG(...) console.log(LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif
#define LOG_INFO(...) console.log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) console.log(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) console.log(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_CRIT(...) console.log(LOG_LEVEL_CRIT, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_CRIT(...)
#define LOG_TRACE(...)
#endif
