// TonePlayer.cpp
//

#include "abstract/TonePlayer.h"




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
}

void TonePlayer::play(int freq)
{
}




//////////////////////////////////////////////////////////////////////////////
//

ITonePlayer* CreateTonePlayer()
{
    static TonePlayer player;
    return &player;
}
