// main.cpp
//

#include <Arduino.h>
#include <M5EPD.h>
#include "board.h"
#include "utils.h"
#include "timezone.h"
#include "logger.h"
#include "agl.h"
#include "CriticalSection.h"
#include "SdCard.h"
#include "NmeaParser.h"
#include "Keypad.h"
#include "Window.h"
#include "Widgets.h"
#include "VarioSentence.h"
#include "VarioLogger.h"
#include "Application.h"
#include "BLEVario.h"


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
