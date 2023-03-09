// TonePlayer.cpp
//

#include <Arduino.h>
#include "abstract/TonePlayer.h"
#include "bsp.h"





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
    //pwm.begin(SineGenerator::USE_CHANNEL_1, SineGenerator::SCALE_HALF);
    
}

void TonePlayer::play(int freq)
{
    //pwm.setFrequency(freq);
    dac1.outputCW(freq);
}




//////////////////////////////////////////////////////////////////////////////
//

ITonePlayer* CreateTonePlayer()
{
    static TonePlayer player;
    return &player;
}
