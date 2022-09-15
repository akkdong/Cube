// Barometer.cpp
//

#include <Arduino.h>

#include "Barometer.h"
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
    uint32_t    lastTick;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// class Barometer implementation

Barometer::Barometer() : lastTick(millis())
{
}

bool Barometer::getData(float* p, float* t)
{
    //
    uint32_t tick = millis();
    if (tick - lastTick < 1000 / 25)
        return false;
    lastTick = tick;

    // enter-critical-section
    *p = baro.getPressure();
    *t = baro.getTemperature();
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
