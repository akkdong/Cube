// adc.h
//

#ifndef __ADC_H__
#define __ADC_H__

#include <Arduino.h>


#ifdef __cplusplus
extern "C"
{
#endif


////////////////////////////////////////////////////////////////////////////////
//

esp_err_t   adc_init(void);

float       adc_get_voltage(void);


#ifdef __cplusplus
}
#endif

#endif // __ADC_H__
