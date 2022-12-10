// VariometerEx.cpp
//

#include "board.h"
#include "logger.h"
#include "utils.h"

#include "VariometerEx.h"
#include "Beeper.h"


////////////////////////////////////////////////////////////////////////////////////////
//

VariometerEx::VariometerEx(Beeper& _beeper)
    : TaskBase("Vario", 4 * 1024, 10)
    , beeper(_beeper)
 {

 }

int VariometerEx::begin(IBarometer* baro, IVarioFilter* filter)
{
    int ret = Variometer::begin(baro, filter);

    this->create();

    pressureLazy = 0.0;
    varioLazy = 0.0;
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

    accumulateVario.begin(1000, 25);
    accumulatePressure.begin(1000, 25);

    //uint32_t tick = millis();
    while (1)
    {
        enter();
        int ret = Variometer::update();
        if (ret > 0)
        {
            //LOGv("%u : %f", millis() - tick, this->vario);
            //tick = millis();

            if (accumulateVario.add(this->vario) > 0)
            {
                varioLazy = accumulateVario.get();
                //LOGv("vario-lazy: %f", varioLazy);
            }
            if (accumulatePressure.add(this->pressure) > 0)
            {
                pressureLazy = accumulatePressure.get();
                //LOGv("pressure-lazy: %f", pressureLazy);
            }

            updateStatus = 1;

            //
            beeper.setVelocity(this->vario);
        }
        leave();

        #ifdef ARDUINO
        vTaskDelay(pdMS_TO_TICKS(1));
        #else
        SDL_Delay(1);
        #endif
    }
}
