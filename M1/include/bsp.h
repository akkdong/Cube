// bsp.h
//

#ifndef __CODE_BSP_H__
#define __CODE_BSP_H__

#ifdef __cplusplus
extern "C" {

#endif

/////////////////////////////////////////////////////////////////////////
//

void bsp_hal_init();
void bsp_gui_init();

void bsp_power_on(bool on);
void bsp_lcd_backlight(bool on);

void bsp_update();


#ifdef __cplusplus
}
#endif

#endif // __CODE_BSP_H__