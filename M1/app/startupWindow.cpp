// startupWindow.cpp
//

#include "startupWindow.h"
#include "flightWindow.h"


///////////////////////////////////////////////////////////////////////////////////
// class StartupWindow

StartupWindow::StartupWindow()
{

}


void StartupWindow::onActivate() 
{
    // start timer
    lv_async_call(_onInitialize, this);
}


void StartupWindow::_onInitialize(void* userData) 
{
    StartupWindow* startup = (StartupWindow *)userData;

    // initizlie
    // ...

    FlightWindow* flight = new FlightWindow;
    Screen::instance()->switchWindow(flight);
}    
