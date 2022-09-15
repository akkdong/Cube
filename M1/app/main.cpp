// main.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <stdint.h>

#include "device_defines.h"
#include "logger.h"
#include "bsp.h"
#include "Application.h"


// call-stack : before
#if 0
    // sim
    srand();
    // dev
    bsp_init();
        // pin-mode
        // setup: serial, wire, pwm, baro, touch
    bsp_power_on(true);

    // dev & sim
    lv_init();
    lv_hal_setup();
        // dev: gfx, lv_disp, lv_indev
        // sim: lv_disp, lv_indev, sdl

    app_config_init(&app_conf);
    app_inint();

    // dev
    bsp_lcd_backlight(true);
    // dev & sim
    lv_hal_loop();
        // sim only
        lv_hal_update();
        app_update();
        lv_task_handler();
        delay // dev: millis, sim: SDL_Delay
#endif


// call-stack : now
#if 0
main
    bsp_hal_init
        pin-mode
        init serial, wire, beep, baro, touch		
    bsp_power_on
    bsp_gui_init
        lv_init
        lv_disp_init
    bsp_lcd_backlight
    
    app.begin
        app_config_init
        app_init
        
    app.run
        bsp_update
            lv_timer_handler
            delay(xx)
        app_update
#endif



#ifndef ARDUINO

// arduino framework entry-point
//

void setup();
void loop();


// simulator entry-point
//

int main(void)
{
    //
    setup();
    loop();

    return 0;
}

#endif


//
//
//

void setup()
{
    // initialize core & peripherals
    bsp_hal_init();
    // hold-on power: turn-on power-switch
    bsp_power_on(true);

    // initialzie lvgl, display, indev, ...
    bsp_gui_init(); 
    // turn-on lcd backlight
    bsp_lcd_backlight(true);

    //
    Application app;
    //app.begin();

    while (1)
    {
        // timer/task handler
        bsp_update();

        //
        app.update();
    }
}


void loop()
{
    // never comes here
}
