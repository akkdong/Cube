// startupWindow.h
//

#ifndef __STARTUP_WINDOW_H__
#define __STARTUP_WINDOW_H__

#include "screen.h"


///////////////////////////////////////////////////////////////////////////////////
// class StartupWindow

class StartupWindow : public Window
{
public:
    StartupWindow();
    virtual ~StartupWindow();

public:
    void            onCreate() override;
    void            onActive() override;

protected:
    bool            process();
    void            start();

    static void     _onSwitch(void* userData);
    static void     _onTimer(lv_timer_t* timer);
};


#endif // __STARTUP_WINDOW_H__
