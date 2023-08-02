// Battery.cpp
//

#include <Arduino.h>
#include "driver/adc.h"
#include "soc/adc_channel.h"

#include "Battery.h"

#define BAT_ADC_CHANNEL     ADC1_GPIO35_CHANNEL
#define BASE_VOLATAGE       3600
#define SCALE               0.5 // 0.5, 0.78571429
#define ADC_FILTER_SAMPLE   10


///////////////////////////////////////////////////////////////////////////
// class BatteryADC

BatteryADC::BatteryADC() : mVoltage(0), mSumValue(0), mSumCount(0), mCalChars(nullptr)
{

}

BatteryADC::~BatteryADC()
{
    if (mCalChars)
        free(mCalChars);
}


int BatteryADC::begin()
{
    if (mCalChars)
        return -1;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BAT_ADC_CHANNEL, ADC_ATTEN_DB_11);
    mCalChars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, BASE_VOLATAGE, mCalChars);

    mVoltage = 0;
    mSumValue = 0;
    mSumCount = 0;

    return 0;
}

void BatteryADC::end()
{
    if (mCalChars)
    {
        free(mCalChars);
        mCalChars = nullptr;
    }
}

int BatteryADC::update()
{
    if (!mCalChars)
        return -1;

    uint32_t rawValue = adc1_get_raw(BAT_ADC_CHANNEL);

    mSumValue += rawValue;
    mSumCount += 1;

    if (mSumCount < ADC_FILTER_SAMPLE)
        return 0;

    // update voltage: average value
    uint32_t avgValue = mSumValue / ADC_FILTER_SAMPLE;

    mVoltage = (uint32_t)(esp_adc_cal_raw_to_voltage(avgValue, mCalChars) / SCALE);
    mSumValue = 0;
    mSumCount = 0;        

    return 1;
}
