// TonePlayer.cpp
//

#include <Arduino.h>

#include "board.h"

#include "abstract/TonePlayer.h"
#include "VarioSoundGenerator.h"
#include "AudioOutputI2SEx.h"



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
    VarioSoundGenerator gen;
    AudioOutputI2SEx out;
};




//////////////////////////////////////////////////////////////////////////////
//

TonePlayer::TonePlayer()
{
}

void TonePlayer::setup()
{
    out.SetPinout(GPIO_I2S_MCLK, GPIO_I2S_SCLK, GPIO_I2S_LRCK, GPIO_I2S_DOUT);
    out.SetChannels(1);
    out.SetOutputModeMono(true);

    gen.begin(&out, 24000);
}

void TonePlayer::play(int freq)
{
    gen.play(freq);
}



//////////////////////////////////////////////////////////////////////////////
//

ITonePlayer* CreateTonePlayer()
{
    static TonePlayer player;
    return &player;
}
