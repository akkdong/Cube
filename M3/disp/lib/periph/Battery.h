// Battery.h
//

#ifndef __BATTERY_ADC_H__
#define __BATTERY_ADC_H__

#include "esp_adc_cal.h"

#define BIAS_VOLTAGE        (0.0)

///////////////////////////////////////////////////////////////////////////
// class BatteryADC

class BatteryADC
{
public:
    BatteryADC();
    ~BatteryADC();

public:
    //
    int begin();
    void end();
    
    int update();

    //
    float getVoltage() { return mVoltage / 1000.0 + BIAS_VOLTAGE; }

protected:
    uint32_t mVoltage;

    uint32_t mSumValue;
    uint32_t mSumCount;

    esp_adc_cal_characteristics_t *mCalChars;
};

#endif // __BATTERY_ADC_H__
