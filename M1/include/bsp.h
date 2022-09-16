// bsp.h
//

#ifndef __CODE_BSP_H__
#define __CODE_BSP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>


/////////////////////////////////////////////////////////////////////////
//

void bsp_hal_init();
void bsp_gui_init();

void bsp_power_on(bool on);
void bsp_lcd_backlight(bool on);

void bsp_update();


//
void bsp_set_drivers(lv_disp_t* disp, lv_indev_t* touch, lv_indev_t* keypad);
void bsp_regiter_keypad_receiver(lv_obj_t* obj);


#ifdef __cplusplus
}
#endif

#endif // __CODE_BSP_H__