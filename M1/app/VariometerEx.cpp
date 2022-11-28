// VariometerEx.cpp
//

#ifdef ARDUINO

#include <Arduino.h>

#include "device_defines.h"
#include "logger.h"

#include "VariometerEx.h"



////////////////////////////////////////////////////////////////////////////////////////
//

int VariometerEx::begin(IBarometer* baro, IVarioFilter* filter)
{
    int ret = Variometer::begin(baro, filter);

    create();
    updateStatus = 0;

    return ret;
}

int VariometerEx::update()
{
    return updateStatus;
}

void VariometerEx::resetUpdate()
{
    updateStatus = 0;
}

void VariometerEx::TaskProc()
{
    uint32_t tick = millis();
    while (1)
    {
        int ret = Variometer::update();
        if (ret > 0)
        {
            //LOGi("[v] %u", millis() - tick);
            tick = millis();

            updateStatus = 1;
        }

        vTaskDelay(1);
    }
}

#endif // ARDUINO
