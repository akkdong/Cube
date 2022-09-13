// logger.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "logger.h"

//
//
//

static const char* trace_prefix = "EWIVD";
static char trace_buf[256];


int _trace_printf(int level, const char* format, ...)
{
    if (level < LOG_LEVEL_ERROR)
        level = LOG_LEVEL_ERROR;
    if (level > LOG_LEVEL_DEBUG)
        level = LOG_LEVEL_DEBUG;

    va_list args;
    va_start(args, format);
    vsprintf(trace_buf, format, args);
    va_end(args);

    return printf("[%c] %s\n", trace_prefix[level], trace_buf);
}
