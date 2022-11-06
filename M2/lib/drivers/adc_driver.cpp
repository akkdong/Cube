// adc.cpp
//

#include "board.h"
#include "adc_driver.h"

#include "driver/adc_common.h"
#include "esp_adc_cal.h"
#include "esp_event.h"

//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif


static const adc_atten_t atten_ = ADC_ATTEN_DB_11;

static adc_unit_t unit = ADC_UNIT_2;
static adc_channel_t channel = ADC_CHANNEL_7;  // ADC2_CHANNEL_7, GPIO18
static esp_adc_cal_characteristics_t adc_chars;


static esp_err_t adc_calibration_init(adc_unit_t unit, adc_atten_t atten)
{
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP_FIT) != ESP_OK)
        return ESP_FAIL;

    esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, 0, &adc_chars);

    return ESP_OK;
}

esp_err_t adc_init()
{
    esp_err_t err = ESP_OK;

    if ((err = adc_calibration_init(unit, ADC_ATTEN_DB_11)) == ESP_OK)
    {
        if (unit == ADC_UNIT_1)
            err = adc1_config_channel_atten((adc1_channel_t)channel, (adc_atten_t)ADC_ATTEN_DB_11);
        else if (unit == ADC_UNIT_2)
            err = adc2_config_channel_atten((adc2_channel_t)channel, (adc_atten_t)ADC_ATTEN_DB_11);
    }

    return err;
}

float adc_get_voltage()
{
    esp_err_t err = ESP_OK;
    int value = 0;
    if (unit == ADC_UNIT_1)
        value = adc1_get_raw((adc1_channel_t)channel);
    else if (unit == ADC_UNIT_2)
        err = adc2_get_raw((adc2_channel_t)channel, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, &value);

    if (err == ESP_OK)
    {
        uint32_t mv = esp_adc_cal_raw_to_voltage(value, &adc_chars);

        return mv * 2.0f / 1000;
    }

    return 0.0f;
}
