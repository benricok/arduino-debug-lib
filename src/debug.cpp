/* based on https://github.com/arcao/Syslog
MIT License

Copyright (c) 2024 Benrico Krog

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "debug.h"

#ifdef CONSOLE

Console console;

/**
 * @brief Console Constructor
 *
 * @note
 * Initialize serial debug port to SERIAL_BAUD speed, default = 115200
 * @param waitForConnection Optional
 */
Console::Console(bool waitForConnection)
{
    Serial.begin(SERIAL_BAUD);

    while (!Serial.available() && waitForConnection)
    {
        delay(100);
    }
}

/**
 * @brief Destructor
*/
Console::~Console() {
    Serial.end();
}

/**
 * @brief like printf but va_list based
 *
 * @note
 * If the resulting string is longer than sizeof(printBuf)-1 characters, the remaining
 * characters are still counted for the return value
 * @param format format string
 * @param arg va_list arguments
 */
size_t Console::vprintf(const char *format, va_list arg)
{
    va_list copy;
    static char printBuf[160];

    va_copy(copy, arg);
    size_t len = vsnprintf(printBuf, sizeof(printBuf), format, copy);
    va_end(copy);

    // If the resulting string is longer than sizeof(printBuf)-1 characters, the remaining
    // characters are still counted for the return value

    if (len > sizeof(printBuf) - 1)
    {
        len = sizeof(printBuf) - 1;
        printBuf[sizeof(printBuf) - 2] = '\n';
    }

    len = Serial.write(printBuf, len);
    return len;
}

/**
 * @brief Debug logging print message
 *
 * @note
 * If the provide format string ends with a newline we assume it is the final print of a single
 * log message. Otherwise we assume more prints will come before the log message ends.  This
 * allows you to call logDebug a few times without printing the header for every line if you wish.
 *
 * @param logLevel Current message loglevel
 * @param format printf like syntax
 * @return Number of characters printed
 */
size_t Console::log(const char *logLevel, const char *format, ...)
{
    size_t r = 0;

    va_list arg;
    va_start(arg, format);

    // Cope with 0 len format strings, but look for new line terminator
    bool hasNewline = *format && format[strlen(format) - 1] == '\n';
    // If we are the first message on a report, include the header
    if (!isContinuosMessage)
    {
        if (timeStatus() == timeSet)
        {
            r += Serial.printf("%s | %02d:%02d:%02d %u | ", logLevel, hour(), minute(), second(), millis() / 1000);
        }
        else
            r += Serial.printf("%s | ??:??:?? %u | ", logLevel, millis() / 1000);
    }
    else
        r += Serial.printf("%22s | ", "");
    r += vprintf(format, arg);
    va_end(arg);

    isContinuosMessage = !hasNewline;

    return r;
}

/**
 * @brief Print out hex values of a buffer into the console
 *
 * @param logLevel Current message loglevel
 * @param buf Buffer to dump
 * @param size Buffer size
 */
void Console::hexDump(const char *logLevel, unsigned char *buf, size_t size)
{
    const char a[17] = "0123456789ABCDEF";
    log(logLevel, "   +------------------------------------------------+ +----------------+");
    log(logLevel, "   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F | |      ASCII     |");
    for (size_t i = 0; i < size; i += 16)
    {
        if (i % 128 == 0)
        {
            log(logLevel, "   +------------------------------------------------+ +----------------+");
        }
        char s[] = "|                                                | |                |";
        size_t ix = 1, iy = 52;
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < size)
            {
                byte c = buf[i + j];
                s[ix++] = a[(c >> 4) & 0x0F];
                s[ix++] = a[c & 0x0F];
                ix++;
                if (c > 31 && c < 128)
                    s[iy++] = c;
                else
                    s[iy++] = '.';
            }
        }
        size_t index = i / 16;
        char c = ""
        if (i < 256)
            c = " ";
        log(logLevel, "%s%02x.%s",c, index, s);
    }
    log(logLevel, "   +------------------------------------------------+ +----------------+\n");
}

#endif
