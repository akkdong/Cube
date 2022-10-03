// bsp.cpp
//

#include <unistd.h>
#define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <lvgl.h>
#include "lv_disp.h"

#include "bsp.h"
#include "ble_vario.h"


//
//
//

void bsp_hal_init()
{
    srand(time(NULL));
}

void bsp_power_on(bool on)
{
}

void bsp_lcd_backlight(bool on)
{
}


//
//
//

void bsp_gui_init()
{
    //
    lv_init();
    lv_disp_init();
}



//
//
//

void bsp_update()
{
	lv_timer_handler();
	SDL_Delay(5);
}



//
//
//

static lv_disp_t*	cube_disp = NULL;
static lv_indev_t*	cube_touch = NULL;
static lv_indev_t*	cube_keypad = NULL;

void bsp_set_drivers(lv_disp_t* disp, lv_indev_t* touch, lv_indev_t* keypad)
{
	cube_disp = disp;
	cube_touch = touch;
	cube_keypad = keypad;
}

void bsp_regiter_keypad_receiver(lv_obj_t* obj)
{
	if (cube_keypad != NULL)
	{
		lv_group_t* grp = lv_group_create();
		lv_group_add_obj(grp, obj);

		lv_indev_set_group(cube_keypad, grp);	
	}
}




//
//
//

bool ble_isConnected()
{
	return false;
}

int  ble_writeBuffered(uint8_t ch)
{
	return 0;
}

void ble_flush()
{
	// nop
}

size_t ble_press(uint8_t key)
{
	return 0;
}

size_t ble_release(uint8_t key)
{
	return 0;
}
