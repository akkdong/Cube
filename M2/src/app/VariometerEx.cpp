// VariometerEx.cpp
//

#include "board.h"
#include "logger.h"
#include "utils.h"

#include "VariometerEx.h"



////////////////////////////////////////////////////////////////////////////////////////
//

int VariometerEx::begin(IBarometer* baro, IVarioFilter* filter)
{
    int ret = Variometer::begin(baro, filter);

    task.setName("Vario");
    task.setStackSize(4 * 1024);
    task.setPriority(10);
    task.create(this);

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
    LOGd("VariometerEx::TaskProc()");
    //uint32_t tick = millis();
    while (1)
    {
        //enter();
        int ret = Variometer::update();
        //leave();

        if (ret > 0)
        {
            //LOGi("[v] %u", millis() - tick);
            //tick = millis();

            updateStatus = 1;
        }

        #ifdef ARDUINO
        vTaskDelay(1 / portTICK_PERIOD_MS);
        #else
        SDL_Delay(1);
        #endif
    }
}
