// Beeper.h
//

#ifndef __BEEPER_H__
#define __BEEPER_H__

#include "abstract/TonePlayer.h"


////////////////////////////////////////////////////////////////////////////////////
//

// Tone
//            +----------------------+------+
//            | play                 | mute |
//            +----------------------+------+
//   start -->|                      |      |
//            |--------------------->| active
//            |---------------------------->| length

struct Tone
{
    Tone() : freq(0), active(1000), length(1000) {}

    Tone(int f, int act, int len) : freq(f), active(act), length(len) {}

    Tone(const Tone& tone) {
        freq = tone.freq;
        active = tone.active;
        length = tone.length;
    }

    void reset() {
        freq = 0;
        active = 1000;
        length = 1000;
    }

	int			freq;       // PWM frequency, 0: mute
    int         active;     // play duration
	int			length;     // tone duration : active <= length
};

typedef struct Tone * TonePtr;




////////////////////////////////////////////////////////////////////////////////////
// class Beeper

class Beeper
{
public:
    Beeper();

    enum State {
        MUTE,
        PLAY,
        HOLD,
    };

public:
    void            begin(ITonePlayer* tp);
    void            end();

    void            update();

    void            setVelocity(float vel);
    void            setMute();

protected:
    Tone            getTone(float vel);
    void            findTone(float velocity, int& freq, int& period, int& duty);

protected:
    ITonePlayer*    tonePlayer;
    State           toneState;

    Tone*           toneAct;
    Tone            toneCur;
    Tone            toneNext;

    uint32_t        tickStart;

};



#endif // __BEEPER_H__
