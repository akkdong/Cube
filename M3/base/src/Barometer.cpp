// Barometer.cpp
//

#include <Arduino.h>

#include "abstract/Barometer.h"
#include "bsp.h"
#include "logger.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// class Barometer declaration

class Barometer : public IBarometer
{
public:
    Barometer();

public:
    bool getData(float* p, float* t);

protected:
    #if USE_UPDATE_TICK
    uint32_t    lastTick;
    #endif
};


///////////////////////////////////////////////////////////////////////////////////////////////
// class Barometer implementation

Barometer::Barometer() 
    #if USE_UPDATE_TICK
    : lastTick(millis())
    #endif
{
}

bool Barometer::getData(float* p, float* t)
{
    //
    #if USE_UPDATE_TICK
    uint32_t tick = millis();
    if (tick - lastTick < 1000 / 25)
        return false;
    lastTick = tick;
    #endif

    // enter-critical-section
    *t = baro.getTemperature();
    *p = baro.getPressure();
    // leave-critical-section

    return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////
//

IBarometer* CreateBarometer()
{
    static Barometer _baro;
    return &_baro;
}
