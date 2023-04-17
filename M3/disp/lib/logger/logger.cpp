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

class MutualLock
{
public:
    MutualLock() {
        mutex = xSemaphoreCreateMutex();
    }

    ~MutualLock() {
        vSemaphoreDelete(mutex);
    }

    void enter() {
        xSemaphoreTake(mutex, portMAX_DELAY);
    }

    void leave() {
        xSemaphoreGive(mutex);
    }

    SemaphoreHandle_t mutex;
};


//
//
//

static const char* trace_prefix = "EWIVD";
static char trace_buf[256];

static Stream& Debug = Serial;
static MutualLock lock;


//
//
//

int trace_putc(uint8_t c)
{
    lock.enter();
    int ret = Debug.write(c);
    lock.leave();

    return ret;
}

int trace_puts(const char* str)
{
    lock.enter();
    int ret = Debug.write(str);
    lock.leave();

    return ret;
}

int trace_printf(const char* format, ...)
{
    lock.enter();
    va_list args;
    va_start(args, format);
    vsprintf(trace_buf, format, args);
    va_end(args);
    int ret = Debug.write(trace_buf);
    lock.leave();

    return ret;
}


int _log_printf(int level, const char* format, ...)
{
    if (level < LOG_LEVEL_ERROR)
        level = LOG_LEVEL_ERROR;
    if (level > LOG_LEVEL_DEBUG)
        level = LOG_LEVEL_DEBUG;

    lock.enter();
    va_list args;
    va_start(args, format);
    vsprintf(trace_buf, format, args);
    va_end(args);
    int ret = Debug.printf("[%c] %s\r\n", trace_prefix[level], trace_buf);
    lock.leave();

    return ret;
}
