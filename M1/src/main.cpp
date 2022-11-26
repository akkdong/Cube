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

#include "Application.h"

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
    Application app;

    app.begin();

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

