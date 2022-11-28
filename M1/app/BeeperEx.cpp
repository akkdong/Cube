// BeeperEx
//

#ifdef ARDUINO

#include "BeeperEx.h"



///////////////////////////////////////////////////////////////////
// BeeperEx implementation

BeeperEx::BeeperEx() 
    : TaskBase("Beeper", 4 * 1024, 5)
    , lockHandle(NULL) 
{
}

BeeperEx::~BeeperEx()
{
    if (lockHandle != NULL)
        vSemaphoreDelete(lockHandle);
}

void BeeperEx::begin(ITonePlayer* tp)
{
    Beeper::begin(tp);

    create();

    if (lockHandle == NULL)
        lockHandle = xSemaphoreCreateMutex();
}

void BeeperEx::end()
{
    Beeper::end();

    if (lockHandle != NULL)
    {
        vSemaphoreDelete(lockHandle);
        lockHandle = NULL;
    }
}


void BeeperEx::update()
{
    lock();
    Beeper::update();
    unlock();
}


void BeeperEx::setVelocity(float vel)
{
    lock();
    Beeper::setVelocity(vel);
    unlock();
}

void BeeperEx::setMute()
{
    lock();
    Beeper::setMute();
    unlock();
}


void BeeperEx::playMelody(Tone* tones, int toneCount)
{
    lock();
    Beeper::playMelody(tones, toneCount);
    unlock();
}


void BeeperEx::TaskProc()
{
    while (1)
    {
        update();

        vTaskDelay(5);
    }
}

#endif // ARUDINO