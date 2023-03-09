// bsp.h
//

#ifndef __BSP_H__
#define __BSP_H__

#include <Arduino.h>
#include <stdint.h>

#include "config.h"
#include "board.h"
#include "DacESP32.h"
#include "bme280.h"


#ifdef __cplusplus
extern "C"
{
#endif 

///////////////////////////////////////////////////////////////////////////////////////
//

void    bsp_hal_init();
void    bsp_gui_init();

void    bsp_power_on(bool on);
void    bsp_lcd_backlight(bool on);

void    bsp_update();


///////////////////////////////////////////////////////////////////////////////////////
//

extern Stream &         SerialDebug;
extern Stream &         SerialHost;
extern Stream &         SerialGPS;

extern Bme280TwoWire	baro;
extern DacESP32         dac1;
extern DacESP32         dac2;


#ifdef __cplusplus
}
#endif

#endif // __BSP_H__
