// Beeper.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "board.h"
#include "config.h"
#include "utils.h"
#include "Beeper.h"


////////////////////////////////////////////////////////////////////////////////////
//

struct VarioTone
{
	float			velocity;
	
	uint16_t		freq;
	uint16_t		period;
	uint16_t		duty;
};


const VarioTone toneTable[] =
{
#if 0    
    // y = exp((x + 320) / 180) x 0.8 + 800
    //
	{  -10.00,   200, 200,  100 },
	{   -3.00,   290, 200,  100 },
	{   -2.00,   369, 200,  100 },
	{   -1.00,   440, 200,  100 },
	{   -0.50,   470, 600,  100 },
	{    0.00,   523, 800,   50 },	// C5
	{    1.00,   622, 500,   54 },	// D5S
	{    2.00,   698, 400,   58 },	// F5
	{    4.00,   784, 275,   62 },	// G5
	{    6.00,   880, 250,   66 },	// A5
	{    8.00,   988, 225,   68 },	// B5
	{   10.00,  1047, 200,   70 },	// C6
#else
    #if 0
	{  -10.00,   200, 200,  100 },
	{   -3.00,   290, 200,  100 },
	{   -2.00,   369, 200,  100 },
	{   -1.00,   440, 200,  100 },
	{   -0.50,   470, 800,  100 },
	{    0.00,   520, 800,   50 },
	{    1.00,   590, 720,   54 },
	{    2.00,   650, 640,   58 },
	{    4.00,   790, 500,   62 },
	{    6.00,   940, 380,   66 },
	{    8.00,  1080, 280,   68 },
	{   10.00,  1200, 200,   70 },
    #else
    {  -10.00,	 200,	100,	100 },
	{  -3.00,	 280,	100,	100 },
	{  -0.51,	 300,	500,	100 },
	{  -0.50,	 200,	800,	  5 },
	{   0.09,	 400,	600,	 10 },
	{   0.10,	 400,	600,	 50 },
	{   1.16,	 550,	552,	 52 },
	{   2.67,	 764,	483,	 55 },
	{   4.24,	 985,	412,	 58 },
	{   6.00,	1234,	322,	 62 },
	{   8.00,	1517,	241,	 66 },
	{  10.00,	1800,	150,	 70 },
    #endif
#endif
};



////////////////////////////////////////////////////////////////////////////////////
// class Beeper

Beeper::Beeper() : tonePlayer(NULL)
{
}

void Beeper::begin(ITonePlayer* tp)
{
    //
    tonePlayer = tp;

    tonePlayer->setup();
    tonePlayer->play(0);

    //
    toneAct = NULL;
    toneState = MUTE;

    melodyPtr = NULL;
    melodyAct = 0;
    melodyLen = 0;
}

void Beeper::end()
{
}

void Beeper::update()
{
    if (toneAct)
    {
        uint32_t tick = get_tick();
        uint32_t duration = tick - tickStart;

        if (toneState == PLAY)
        {
            if (duration > toneAct->active && duration < toneAct->length)
            {
                tonePlayer->play(0);
                toneState = HOLD;
            }
        }

        if (duration > toneAct->length)
        {
            if (toneState == PLAY)
                tonePlayer->play(0);

            toneAct = NULL;
            toneState = MUTE;
        }
    }

    if (!toneAct && melodyPtr != NULL && melodyAct < melodyLen)
    {
        toneCur = melodyPtr[melodyAct++];
        toneAct = &toneCur;

        //
        tonePlayer->play(toneAct->freq);
        tickStart = get_tick();
        toneState = PLAY;
    }

    if (!toneAct && toneNext.freq != 0)
    {
        //
        toneCur = toneNext;
        toneNext.reset();

        toneAct = &toneCur;

        //
        tonePlayer->play(toneAct->freq);
        tickStart = get_tick();
        toneState = PLAY;
    }
}

void Beeper::setVelocity(float vel)
{
    toneNext = getTone(vel);
}

void Beeper::setMute()
{
    tonePlayer->play(0);

    toneAct = NULL;
    toneState = MUTE;
    toneCur.reset();
    toneNext.reset();
}

void Beeper::playMelody(Tone* tones, int toneCount)
{
    melodyPtr = tones;
    melodyAct = 0;
    melodyLen = toneCount;
}

Tone Beeper::getTone(float vel)
{
    int freq, period, duty;

    if ((vel < -3.0) || (vel > 0.2) || (vel > 0.0 && toneState == PLAY))
    {
        findTone(vel, freq, period, duty);
        int active = (int)(period * duty / 100.0f);
        return Tone(freq, active, period);
    }

    return Tone(0, 1000, 1000);
}

#define _count_of(x)		(sizeof(x) / sizeof(x[0]))

void Beeper::findTone(float velocity, int& freq, int& period, int& duty)
{
	int index;
	
	for (index = 0; index < _count_of(toneTable); index++)
	{
		if (velocity <= toneTable[index].velocity)
			break;
	}
	
	if (index == 0)
	{
		freq = toneTable[index].freq;
		period = toneTable[index].period;
		duty = toneTable[index].duty;
	}
    else if (index == _count_of(toneTable))
    {
		freq = toneTable[index-1].freq;
		period = toneTable[index-1].period;
		duty = toneTable[index-1].duty;
    }
	else
	{
		float ratio = (velocity - toneTable[index-1].velocity) / (toneTable[index].velocity - toneTable[index-1].velocity);
		
		freq = (toneTable[index].freq - toneTable[index-1].freq) * ratio + toneTable[index-1].freq;
		period = (toneTable[index].period - toneTable[index-1].period) * ratio + toneTable[index-1].period;
		duty = (toneTable[index].duty - toneTable[index-1].duty) * ratio + toneTable[index-1].duty;
	}
	
    //LOGv("findTone: %d, %d, %d\n", freq, period, duty);
}