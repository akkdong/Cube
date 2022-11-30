// Display.cpp
//

#include "Display.h"

#include "utils.h"
#include "logger.h"
#include "bsp.h"

#include "screen.h"


//////////////////////////////////////////////////////////////////
// class Display

Display::Display()
    : timerHandler(this)
    , updateHandler(this)
    , dirty(false)
{

}

Display::~Display()
{

}


int Display::begin()
{
    // initialzie lvgl, display, indev, ...
    bsp_gui_init(); 
    // turn-on lcd backlight
    bsp_lcd_backlight(true);

    //
    taskTimerHandler.setName("timer");
    taskTimerHandler.setStackSize(4 * 1024);
    taskTimerHandler.setPriority(3);    
    taskTimerHandler.create(&timerHandler);

    taskUpdateHandler.setName("update");
    taskUpdateHandler.setStackSize(4 * 1024);
    taskUpdateHandler.setPriority(2);
    //taskUpdateHandler.create(&updateHandler);

    return 0;
}

void Display::update()
{
    static uint32_t lastTick = millis();
    uint32_t tick = millis();

    if (isDirty() && (tick - lastTick > 500))
    {
        lastTick = tick;
        setDirty(false);

        enter();
        Window* active = Screen::instance()->peekWindow();
        if (active)
            active->update();
        leave();
    }
}


void Display::TimerHandler::TaskProc()
{
    while (1)
    {
        disp->enter();
        lv_timer_handler();
        disp->leave();

        delay(10);
    }
}

void Display::UpdateHandler::TaskProc()
{
    uint32_t lastTick = millis();

    while (1)
    {
        uint32_t tick = millis();

        if (disp->isDirty() && (tick - lastTick > 500))
        {
            lastTick = tick;
            disp->setDirty(false);

            disp->enter();
            Window* active = Screen::instance()->peekWindow();
            if (active)
                active->update();
            disp->leave();
        }
        else
        {
            delay(10);
        }
    }
}