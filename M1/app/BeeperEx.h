// BeeperEx.h
//

#include <Arduino.h>

#include "Beeper.h"
#include "TaskBase.h"


///////////////////////////////////////////////////////////////////
// class BeeperEx

class BeeperEx : public TaskBase, public Beeper
{
public:
    BeeperEx();
    virtual ~BeeperEx();

public:
    void            begin(ITonePlayer* tp) override;
    void            end() override;

    void            update() override;

    void            setVelocity(float vel) override;
    void            setMute() override;

    void            playMelody(Tone* tones, int toneCount) override;

protected:
    void            TaskProc() override;

    void lock() { 
        if (lockHandle != NULL) 
            xSemaphoreTake(lockHandle, portMAX_DELAY); 
    }
    void unlock() { 
        if (lockHandle) 
        xSemaphoreGive(lockHandle); 
    }

protected:
    SemaphoreHandle_t lockHandle;
};
