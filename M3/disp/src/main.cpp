// main.cpp
//

#include <Arduino.h>
#include <M5EPD.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <WiFiManager.h>

#include "board.h"
#include "utils.h"
#include "timezone.h"
#include "logger.h"
#include "agl.h"
#include "CriticalSection.h"
#include "NmeaParser.h"
#include "Keypad.h"
#include "Window.h"
#include "Widgets.h"
#include "VarioSentence.h"
#include "VarioLogger.h"
#include "Application.h"
#include "BLEVario.h"
#include "GeographicLib/Geodesic.hpp"
#include "WebService.h"


//
//
//
//
//




//
//
//
//
//

Application App;


Application* Application::getApp()
{
    return &App;
}


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

    // test
    {
        double lat = 37.0;
        double lon = 120.0;

        GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();

        double dist;
        geod.Inverse(lat, lon, lat + 1, lon, dist);
        LOGv("distance from(%f, %f) to(%f, %f) = %f", lat, lon, lat + 1, lon, dist);
    }
}

void loop()
{
    //
    //
    //
    App.update();
}
