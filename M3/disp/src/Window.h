// Window.h
//

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Widgets.h"

enum FallbackResult {
    RES_NONE = 0,
    RES_CANCEL,
    RES_NO,
    RES_YES,
    RES_OK = RES_YES,
    RES_RETRY,
};

class Application;
class Screen;

/////////////////////////////////////////////////////////////////////////////////
// class Window

class Window : public Widget
{
    friend class Application;
    friend class Screen;
public:
    // constructor & destructor
    //Window();
    Window(M5EPD_Canvas* pRefCanvas);
    Window(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    // operations
    bool setTimer(uint32_t id, uint32_t interval);
    void killTimer(uint32_t id);

protected:
    //
    virtual void onActive() {}
    virtual void onClose() {}
    virtual void onFallback(FallbackResult result, Window *from) {}

    virtual void onMessage(uint32_t code, uint32_t data) {}
    virtual void onTimer(uint32_t id) {}

    virtual void onIdle();

protected:
    struct TimerInfo {
        TimerInfo() {
            id = (uint32_t)-1;
            interval = 0;
            lastTick = 0;
        }

        uint32_t id;
        uint32_t interval;
        uint32_t lastTick;
    };

    TimerInfo m_timer[5];
};


#endif // __WINDOW_H__
