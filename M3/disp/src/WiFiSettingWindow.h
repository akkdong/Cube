// WiFiSettingWindow.h
//

#ifndef __WIFI_SETTING_WINDOW__
#define __WIFI_SETTING_WINDOW__

#include <stdint.h>
#include "Window.h"


/////////////////////////////////////////////////////////////////////////////////
// class WiFiSettingWindow

class WiFiSettingWindow : public Window
{
public:
    WiFiSettingWindow(M5EPD_Canvas *pRefCanvas);

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

    //
    static void WiFiTask(void *param);
    void WiFiTask();


protected:
    TaskHandle_t taskWiFi;
};

#endif // __WIFI_SETTING_WINDOW__
