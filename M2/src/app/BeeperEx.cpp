// BeeperEx
//

#include "BeeperEx.h"

#include "logger.h"
#include "utils.h"


///////////////////////////////////////////////////////////////////
// BeeperEx implementation

BeeperEx::BeeperEx() 
    /*: TaskBase("Beeper", 4 * 1024, 5)*/
{
}

BeeperEx::~BeeperEx()
{
}

void BeeperEx::begin(ITonePlayer* tp)
{
    Beeper::begin(tp);

    task.setName("Beeper");
    task.setStackSize(2 * 1024);
    task.setPriority(5);
    task.create(this);
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
    while (1)
    {
        cs.enter();
        Beeper::update();
        cs.leave();

        #ifdef ARDUINO
        vTaskDelay(5 / portTICK_PERIOD_MS);
        #else
        SDL_Delay(5);
        #endif
    }
}
