// WiFiSettingWindow.h
//

#ifndef __WIFI_SETTING_WINDOW__
#define __WIFI_SETTING_WINDOW__

#include <stdint.h>
#include "Window.h"

class DeviceContext;


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
    virtual void onClose();
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


private:
    const char *getWfiFiState(DeviceContext *contextPtr);
    int hitTest(int x, int y);
    void rebootDevice();

protected:
    int aniIndex = 3;
    ButtonClass btnReboot;
    uint32_t m_lastTouch;

    TaskHandle_t taskWiFi;
};

#endif // __WIFI_SETTING_WINDOW__
