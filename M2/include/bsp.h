// bsp.h
//

#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>

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

bool    ble_isConnected();
int     ble_writeBuffered(uint8_t ch);
void    ble_flush();

size_t  ble_press(uint8_t key);
size_t  ble_release(uint8_t key);


#ifdef __cplusplus
}
#endif

#endif // __BSP_H__
