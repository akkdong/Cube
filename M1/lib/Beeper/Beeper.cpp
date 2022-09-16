// Beeper.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "device_defines.h"
#include "config.h"
#include "logger.h"
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
// y = exp((x + 320) / 180) x 0.8 + 800
//
{
	{  -10.00,   200, 200,  100 },
	{   -3.00,   290, 200,  100 },
	{   -2.00,   369, 200,  100 },
	{   -1.00,   440, 200,  100 },
	{   -0.50,   470, 600,  100 },
	{    0.00,   988, 600,   50 },	// B5
	{    1.00,  1175, 400,   54 },	// D6
	{    2.00,  1319, 300,   58 },	// E6
	{    4.00,  1397, 275,   62 },	// F6
	{    6.00,  1568, 250,   66 },	// G6
	{    8.00,  1760, 225,   68 },	// A6
	{   10.00,  1976, 200,   70 },	// B6
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

    if (!toneAct && toneNext.freq != 0 )
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

Tone Beeper::getTone(float vel)
{
    int freq, period, duty;

    if ((vel < -3.0) || (vel > 0.2) || (vel > 0.0 && toneState == PLAY))
    {
        findTone(vel, freq, period, duty);
        return Tone(freq, period * duty / 100, period);
    }

    return Tone(0, 1000, 1000);
}


void Beeper::findTone(float velocity, int& freq, int& period, int& duty)
{
	int index;
	
	for (index = 0; index < (sizeof(toneTable) / sizeof(toneTable[0])); index++)
	{
		if (velocity <= toneTable[index].velocity)
			break;
	}
	
	if (index == 0 || index == (sizeof(toneTable) / sizeof(toneTable[0])))
	{
		if (index != 0)
			index -= 1;
		
		freq = toneTable[index].freq;
		period = toneTable[index].period;
		duty = toneTable[index].duty;
	}
	else
	{
		float ratio = toneTable[index].velocity / velocity;
		
		freq = (toneTable[index].freq - toneTable[index-1].freq) / (toneTable[index].velocity - toneTable[index-1].velocity) * (velocity - toneTable[index-1].velocity) + toneTable[index-1].freq;
		period = (toneTable[index].period - toneTable[index-1].period) / (toneTable[index].velocity - toneTable[index-1].velocity) * (velocity - toneTable[index-1].velocity) + toneTable[index-1].period;
		duty = (toneTable[index].duty - toneTable[index-1].duty) / (toneTable[index].velocity - toneTable[index-1].velocity) * (velocity - toneTable[index-1].velocity) + toneTable[index-1].duty;
	}
	
    //LOGv("findTone: %d, %d, %d\n", freq, period, duty);
}