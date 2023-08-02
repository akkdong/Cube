// main.cpp
//

#include <Arduino.h>
#include <M5EPD.h>
#include "bsp.h"
#include "utils.h"
#include "timezone.h"
#include "logger.h"
#include "NmeaParser.h"
#include "Keypad.h"
#include "Window.h"
#include "Widgets.h"
#include "Application.h"



Application App;


void setup()
{
    //
    // BSP initialize
    //
    Application::initBoard();


    //
    //
    //
    App.begin();
}

void loop()
{
    //
    //
    //
    App.update();
}
