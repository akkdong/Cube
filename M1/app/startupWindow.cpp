// startupWindow.cpp
//

#include <stdint.h>
#include <stdio.h>
#include "logger.h"

#include "startupWindow.h"
#include "flightWindow.h"


///////////////////////////////////////////////////////////////////////////////////
// class StartupWindow

StartupWindow::StartupWindow()
{

}

StartupWindow::~StartupWindow()
{
    LOGv("StartupWindow::~StartupWindow()\n");
}

void StartupWindow::onCreate()
{
    // startup background
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_bg_opa(_this, LV_OPA_COVER, 0);

    //
    lv_obj_t* logo = lv_label_create(_this);
    if (logo)
    {
        lv_label_set_text(logo, "Initialize Cube");
        lv_obj_set_style_text_font(logo, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(logo, lv_color_hex(0xFFFF00), 0);
        lv_obj_align(logo, LV_ALIGN_CENTER, 0, -60);
    }

    lv_obj_t* count = lv_label_create(_this);
    if (count)
    {
        lv_label_set_text(count, "0");
        lv_obj_set_style_text_font(count, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(count, lv_color_hex(0x0000FF), 0);
        lv_obj_align(count, LV_ALIGN_BOTTOM_MID, 0, 0);
    }
}

void StartupWindow::onActive() 
{
    // start timer
    lv_timer_t* timer = lv_timer_create(_onTimer, 1000, this);
    //lv_async_call(_onInitialize, this);
}

bool StartupWindow::process()
{
    static int count = 0;

    lv_obj_t* label = lv_obj_get_child(_this, 1);
    if (label)
    {
        char sz[16];
        sprintf(sz, "%d", count);
        lv_label_set_text(label, sz);
    }

    return ++count < 4 ? true : false;
}

void StartupWindow::start()
{
    FlightWindow* flight = new FlightWindow;
    Screen::instance()->switchWindow(flight);
}


void StartupWindow::_onSwitch(void* userData) 
{
    StartupWindow* startup = (StartupWindow *)userData;
    startup->start();
}    

void StartupWindow::_onTimer(lv_timer_t* timer)
{
    StartupWindow* startup = (StartupWindow *)timer->user_data;
    if (!startup->process())
    {
        lv_timer_del(timer);
        lv_async_call(_onSwitch, startup);
    }
}
