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

public:
    virtual void onActivate();


    static void _onInitialize(void* userData);
};


#endif // __STARTUP_WINDOW_H__
