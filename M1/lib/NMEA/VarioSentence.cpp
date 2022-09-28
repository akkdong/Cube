// VarioSentence.cpp
//

#include <stdint.h>
#include "device_defines.h"
#include "VarioSentence.h"
#include "utils.h"


/////////////////////////////////////////////////////////////////////////////
// class VarioSentence

VarioSentence::VarioSentence(char type) : sentenceType(type)
{
	varioSentence = ((sentenceType == VARIOMETER_LK8_SENTENCE) ?
				(IVarioSentence *)&LK8 : (IVarioSentence *)&LxNav);
				
	lastTick = get_tick();
}
	
void VarioSentence::begin(float height, float vel, float temp, float bat)
{
	varioSentence->begin(height, vel, temp, bat);
}

void VarioSentence::begin(float height, float vel, float temp, float prs, float bat)
{
	varioSentence->begin(height, vel, temp, prs, bat);
}
	
int VarioSentence::available()
{
	return varioSentence->available();
}
	
int VarioSentence::read()
{
	return varioSentence->read();
}

int VarioSentence::checkInterval()
{
	uint32_t tick = get_tick();
	if (! varioSentence->available() && (tick - lastTick) > VARIOMETER_SENTENCE_DELAY)
	{
		lastTick = tick;

		return true;
	}
	
	return false;
}
