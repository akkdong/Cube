// BatteryVoltage.h
//

#ifndef __BATTERYVOLTAGE_H__
#define __BATTERYVOLTAGE_H__

#include <Arduino.h>


///////////////////////////////////////////////////////////////////////////
// class BatteryVoltage

class BatteryVoltage
{
public:
	int						begin();
	int						update();
	
	float					getVoltage();

private:
	int						updateVoltage();
	
private:
	float					voltageMeasured;
	float                   voltageSum;
	int						sampleCount;

	uint32_t				lastTick;
};


inline float BatteryVoltage::getVoltage()
	{ return voltageMeasured; }
	

#endif // __BATTERYVOLTAGE_H__
