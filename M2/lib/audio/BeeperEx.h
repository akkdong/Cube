// BeeperEx.h
//

#include "TaskBase.h"
#include "CriticalSection.h"
#include "Beeper.h"




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

protected:
    CriticalSection cs;
};
