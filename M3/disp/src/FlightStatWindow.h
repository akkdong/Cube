// FlightStatWindow.h
//

#ifndef __FLIGHT_STAT_WINDOW__
#define __FLIGHT_STAT_WINDOW__

#include <stdint.h>
#include "Window.h"


/////////////////////////////////////////////////////////////////////////////////
// class FlightStatWindow

class FlightStatWindow : public Window
{
public:
    FlightStatWindow(M5EPD_Canvas *pRefCanvas);

public:
    //

protected:
    //
    virtual void onActive();
    virtual void onDraw();   
    virtual void onMessage(uint32_t code, uint32_t data);
    virtual void onTimer(uint32_t id);

    virtual void onTouchDown(int x, int y);
    virtual void onTouchMove(int x, int y);
    virtual void onTouchUp(int x, int y);

    virtual void onKeyPressed(uint32_t key);
    virtual void onKeyLongPressed(uint32_t key);
    virtual void onKeyReleased(uint32_t key);    
};

#endif // __FLIGHT_STAT_WINDOW__