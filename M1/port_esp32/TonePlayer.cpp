// TonePlayer.cpp
//

#include <Arduino.h>

#include "abstract/TonePlayer.h"
#include "SineGenerator.h"




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
    SineGenerator   pwm;
};




//////////////////////////////////////////////////////////////////////////////
//

TonePlayer::TonePlayer()
{
}

void TonePlayer::setup()
{
    pwm.begin(SineGenerator::USE_CHANNEL_1, SineGenerator::SCALE_HALF);
}

void TonePlayer::play(int freq)
{
    pwm.setFrequency(freq);
}




//////////////////////////////////////////////////////////////////////////////
//

ITonePlayer* CreateTonePlayer()
{
    static TonePlayer player;
    return &player;
}
