// BatteryVoltage.cpp
//

#include "BatteryVoltage.h"
#include "adc_driver.h"


#define ADC_MEASURE_INTERVAL		(50)
#define ADC_SAMPLES					(1000/50)
#define DEFAULT_VREF    			(3600)


///////////////////////////////////////////////////////////////////////////
// class BatteryVoltage



int BatteryVoltage::begin()
{
	//
	adc_init();

    voltageMeasured = 0.0f;
    voltageSum = 0.0f;
    sampleCount = 0;
	lastTick = millis();
	
	for (int i = 0; i < ADC_SAMPLES; i++)
		updateVoltage();
	
	return 0;
}

int BatteryVoltage::update()
{
	if ((millis() - lastTick) > ADC_MEASURE_INTERVAL)
	{
		lastTick = millis();
		
		return updateVoltage();
	}
	
	return 0;
}

int BatteryVoltage::updateVoltage()
{
    float voltage = adc_get_voltage();
    voltageSum += voltage;
	sampleCount += 1;
	
	if (sampleCount >= ADC_SAMPLES)
	{
		voltageMeasured = voltageSum / sampleCount;
		
		voltageSum = 0.0f;
		sampleCount = 0;
		
		return 1;
	}
	
	return 0;
}
