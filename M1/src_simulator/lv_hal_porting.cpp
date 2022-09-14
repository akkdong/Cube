#include <unistd.h>
#define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>

#include "display/monitor.h"
#include "indev/mouse.h"
#include "indev/mousewheel.h"
#include "indev/keyboard.h"
#include "sdl/sdl.h"

#include "device_defines.h"
#include "logger.h"
#include "lv_hal_porting.h"
#include "lv_app.h"


Uint32 hal_update_tick;



/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static int hal_tick_handler(void * data)
{
    (void)data;

    while(1) 
    {
        SDL_Delay(5);   /*Sleep for 5 millisecond*/
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }

    return 0;
}


void lv_hal_setup(void)
{
    // Workaround for sdl2 `-m32` crash
    // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
    #ifndef WIN32
    setenv("DBUS_FATAL_WARNINGS", "0", 1);
    #endif

    /* Add a display
     * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/

    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buf[SDL_HOR_RES * 10];                          /*Declare a buffer for 10 lines*/
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, SDL_HOR_RES * 10);    /*Initialize the display buffer*/

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);              /*Basic initialization*/
    disp_drv.flush_cb = sdl_display_flush;    /*Used when `LV_VDB_SIZE != 0` in lv_conf.h (buffered drawing)*/
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = SDL_HOR_RES;
    disp_drv.ver_res = SDL_VER_RES;
    //disp_drv.disp_fill = monitor_fill;      /*Used when `LV_VDB_SIZE == 0` in lv_conf.h (unbuffered drawing)*/
    //disp_drv.disp_map = monitor_map;        /*Used when `LV_VDB_SIZE == 0` in lv_conf.h (unbuffered drawing)*/
    lv_disp_drv_register(&disp_drv);

    /* Add the mouse as input device
     * Use the 'mouse' driver which reads the PC's mouse*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);            /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = sdl_mouse_read;       /*This function will be called periodically (by the library) to get the mouse position and state*/
    lv_indev_drv_register(&indev_drv);

    sdl_init();

    /* Tick init.
     * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about how much time were elapsed
     * Create an SDL thread to do this*/
    SDL_CreateThread(hal_tick_handler, "tick", NULL);
}

void lv_hal_update(void)
{
    if (SDL_GetTicks() - hal_update_tick >= 1000)
    {
        //
        app_conf_t* conf = app_get_conf();

        //conf->altitudeGPS = 2100 + (rand() % 400) - 200;
        conf->altitudeBaro = conf->altitudeGPS + (rand() % 100) - 50;
        conf->altitudeAGL = conf->altitudeGPS - (rand() % 500);

        //conf->speedGround = 20 + (rand() % 20) - 10;
        conf->speedVertActive = (rand() % 30) / 10.0;
        conf->glideRatio = (rand() % 80) / 10.0;

        //conf->heading = (conf->heading + 10) % 360;
        conf->bearing = 30;

        conf->timeCurrent = time(NULL);
        conf->timeFly = conf->timeCurrent - conf->timeTakeoff;
        conf->dirty = 1;

        // 
        hal_update_tick = SDL_GetTicks();
    }
}

void lv_hal_loop(void)
{
    //
    hal_update_tick = SDL_GetTicks();

    app_conf_t* conf = app_get_conf();
    conf->timeTakeoff = time(NULL);

    //
    while (1)
    {
        //
        lv_hal_update();
        app_update();

        //
        lv_task_handler();
        SDL_Delay(5);
    }
}
