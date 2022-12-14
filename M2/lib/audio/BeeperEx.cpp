// BeeperEx
//

#include "BeeperEx.h"

#include "logger.h"
#include "utils.h"


///////////////////////////////////////////////////////////////////
// BeeperEx implementation

BeeperEx::BeeperEx() 
    : TaskBase("Beeper", 2 * 1024, 5)
{
}

BeeperEx::~BeeperEx()
{
}

void BeeperEx::begin(ITonePlayer* tp)
{
    Beeper::begin(tp);

    this->create();
}

void BeeperEx::end()
{
    Beeper::end();
}


void BeeperEx::update()
{
}


void BeeperEx::setVelocity(float vel)
{
    cs.enter();
    Beeper::setVelocity(vel);
    cs.leave();
}

void BeeperEx::setMute()
{
    cs.enter();
    Beeper::setMute();
    cs.leave();
}


void BeeperEx::playMelody(Tone* tones, int toneCount)
{
    cs.enter();
    Beeper::playMelody(tones, toneCount);
    cs.leave();
}


void BeeperEx::TaskProc()
{
    #ifdef ARDUINO
    TickType_t prevWakeTime = xTaskGetTickCount();
    #endif

    while (1)
    {
        cs.enter();
        Beeper::update();
        cs.leave();

        #ifdef ARDUINO
        vTaskDelayUntil(&prevWakeTime, pdMS_TO_TICKS(10));
        #else
        SDL_Delay(10);
        #endif
    }
}
