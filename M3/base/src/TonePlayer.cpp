// TonePlayer.cpp
//

#include <Arduino.h>
#include "abstract/TonePlayer.h"
#include "bsp.h"



#define PIN_SD          (16)



//////////////////////////////////////////////////////////////////////////////
//

class TonePlayer : public ITonePlayer
{
public:
    TonePlayer();

public:
    void            setup();
    void            play(int freq);

protected:    
    
};




//////////////////////////////////////////////////////////////////////////////
//

TonePlayer::TonePlayer()
{
}

void TonePlayer::setup()
{
    //
    pinMode(PIN_SD, OUTPUT);
    digitalWrite(PIN_SD, LOW); // mute

    //
    //dac1.enable();
    //dac2.enable();
    dac2.setCwPhase(DAC_CW_PHASE_180);
}

void TonePlayer::play(int freq)
{
    if (freq > 0)
    {
        dac1.outputCW(freq);
        dac2.outputCW(freq);
        digitalWrite(PIN_SD, HIGH);
    }
    else
    {
        digitalWrite(PIN_SD, LOW);
        dac1.disable();
        dac2.disable();
    }
}




//////////////////////////////////////////////////////////////////////////////
//

ITonePlayer* CreateTonePlayer()
{
    static TonePlayer player;
    return &player;
}
