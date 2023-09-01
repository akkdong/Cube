// RouteSettingWindow.h
//

#ifndef __ROUTE_SETTING_WINDOW_H__
#define __ROUTE_SETTING_WINDOW_H__

#include <stdint.h>
#include "Window.h"


/////////////////////////////////////////////////////////////////////////////////
// class RouteSettingWindow

class RouteSettingWindow : public Window
{
public:
    RouteSettingWindow(M5EPD_Canvas *pRefCanvas);

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

#endif // __ROUTE_SETTING_WINDOW_H__
