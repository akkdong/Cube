// IGCLogger.cpp
//

#include <Arduino.h>

#include "IGCLogger.h"


/////////////////////////////////////////////////////////////////////////////
//

IGCLogger::IGCLogger()
{
}

bool IGCLogger::begin(time_t date)
{
    return true;
}

void IGCLogger::end(time_t date)
{
}

int IGCLogger::write(uint8_t ch)
{
    return 1;
}

void IGCLogger::updateBaroAltitude(float varioAlt)
{
}

int IGCLogger::isInitialized()
{
    return 0;
}

int IGCLogger::isLogging()
{
    return 0;
}
