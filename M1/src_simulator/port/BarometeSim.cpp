// BarometerSim.cpp
//

#include <SDL2/SDL.h>
#include <time.h>

#include "Barometer.h"


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
    updateCount += 1;
    // period : 10 sec, tick : 1000 / 25 ms
    // 1 period -> 10000 / (1000 / 25) = 250 tick
    // 1 tick = 2 * 3.14 / 250 rad
    
    *p = 95000 + sin(updateCount * 2 * 3.14 / (10000 / (1000 / 25))) * 60;
    *t = 15.0;

    return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////
//

IBarometer* CreateBarometer()
{
    static BarometerSim sim;
    return &sim;
}
