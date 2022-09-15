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
