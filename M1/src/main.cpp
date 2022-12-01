// main.cpp
//

#include <Arduino.h>

#include "device_defines.h"
#include "logger.h"
#include "bsp.h"

#include "VarioSentence.h"
#include "Variometer.h"
#include "Keypad.h"
#include "Digit.h"
#include "Beeper.h"
#include "Battery.h"
#include "Agl.h"
#include "IGCLogger.h"
#include "TaskWatchdog.h"

#include "Application.h"


//
//
//

Application app;



//
//
//

void setup()
{
    // initialize core & peripherals
    LOGi("Start M1 Simulator!!");
    bsp_hal_init();
    // hold-on power: turn-on power-switch
    bsp_power_on(true);

    // initialzie lvgl, display, indev, ...
    bsp_gui_init(); 
    // turn-on lcd backlight
    bsp_lcd_backlight(true);

    LOGi("Total heap: %d", ESP.getHeapSize());
    LOGi("Free heap: %d", ESP.getFreeHeap());
    LOGi("Total PSRAM: %d", ESP.getPsramSize());
    LOGi("Free PSRAM: %d", ESP.getFreePsram());    

    //
    DeviceRepository& repo = DeviceRepository::instance();
    repo.reset();
    repo.loadPref();    

    //
    //TaskWatchdog::begin(1000);
    //TaskWatchdog::add(NULL);      

    //
    app.begin();

    #if 0
    while (1)
    {
        // timer/task handler
        bsp_update();

        //
        app.update();

        //
        TaskWatchdog::reset();
    }
    #endif
}


void loop()
{
    // timer/task handler
    bsp_update();

    //
    app.update();
}

