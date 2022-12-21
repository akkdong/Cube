// Barometer.cpp
//

#include <Arduino.h>

#include "abstract/Barometer.h"
#include "Bme280.h"
#include "logger.h"


extern Bme280TwoWire baro;

///////////////////////////////////////////////////////////////////////////////////////////////
// class Barometer declaration

class Barometer : public IBarometer
{
public:
    Barometer();

public:
    bool getData(float* p, float* t);

protected:
#if __REMOVED__
    uint32_t    lastTick;
#endif
};


///////////////////////////////////////////////////////////////////////////////////////////////
// class Barometer implementation

Barometer::Barometer() 
    #if __REMOVED__
    : lastTick(millis())
    #endif
{
}

bool Barometer::getData(float* p, float* t)
{
    //
    #if __REMOVED__
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
