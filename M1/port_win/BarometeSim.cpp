// BarometerSim.cpp
//

#include <SDL2/SDL.h>
#include <time.h>

#include "abstract/Barometer.h"
#include "baro_log.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// class BarometerSim declaration

class BarometerSim : public IBarometer
{
public:
    BarometerSim();

public:
    bool getData(float* p, float* t);

protected:
    Uint32     lastTick;
    uint32_t   updateCount;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// class BarometerSim implementation

BarometerSim::BarometerSim() : lastTick(time(NULL)), updateCount(0)
{
}

bool BarometerSim::getData(float* p, float* t)
{
    //
    Uint32 tick = SDL_GetTicks();
    if (tick - lastTick < 1000 / 25)
        return false;
    lastTick = tick;

    //
    #if 1
    updateCount += 1;
    // period : 60 sec, tick : 1000 / 25 ms
    // 1 period -> 10000 / (1000 / 25) = 250 tick
    // 1 tick = 2 * 3.14 / 250 rad
    
    *p = 95000 + sin(updateCount * 2 * 3.14 / (60000 / (1000 / 25))) * 80 + (rand() % 90);
    *t = 15.0;
    #else
    *p = baro_data[updateCount].pressure;
    *t = baro_data[updateCount].temperature;

    updateCount = (updateCount + 1) % (sizeof(baro_data) / sizeof(baro_data[0]));
    #endif

    return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////
//

IBarometer* CreateBarometer()
{
    static BarometerSim sim;
    return &sim;
}
