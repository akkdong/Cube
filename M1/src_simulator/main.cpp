// main.cpp
//

#include <stdint.h>

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
#include "BluetoothManager.h"

#include "Application.h"
#include "Display.h"


Display     disp;
Application app(disp);


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

    #ifdef ARDUINO
    LOGi("Total heap: %d", ESP.getHeapSize());
    LOGi("Free heap: %d", ESP.getFreeHeap());
    LOGi("Total PSRAM: %d", ESP.getPsramSize());
    LOGi("Free PSRAM: %d", ESP.getFreePsram());    
    #endif

    //
    DeviceRepository& repo = DeviceRepository::instance();
    repo.reset();
    repo.loadPref();

    //
    //disp.begin();
    app.begin();

    #if 0
    while (1)
    {
        // timer/task handler
        bsp_update();

        //
        app.update();
    }
    #endif
}


void loop()
{
    // timer/task handler
    bsp_update();
    //disp.update();

    //
    app.update();
}



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

    while (1)
    {
        loop();
    }

    return 0;
}

#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    LOGi("WinMain()");
    return main();
}