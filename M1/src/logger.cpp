// logger.cpp
//

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "logger.h"

//
//
//

static const char* trace_prefix = "EWIVD";
static char trace_buf[256];


int trace_putc(uint8_t c)
{
    return Serial.write(c);
}

int trace_puts(const char* str)
{
    return Serial.write(str);
}

int trace_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(trace_buf, format, args);
    va_end(args);

    return Serial.write(trace_buf);
}


int _log_printf(int level, const char* format, ...)
{
    if (level < LOG_LEVEL_ERROR)
        level = LOG_LEVEL_ERROR;
    if (level > LOG_LEVEL_DEBUG)
        level = LOG_LEVEL_DEBUG;

    va_list args;
    va_start(args, format);
    vsprintf(trace_buf, format, args);
    va_end(args);

    return Serial.printf("[%c] %s\n", trace_prefix[level], trace_buf);
}
