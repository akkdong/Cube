// Screen.h
//

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "Window.h"

#define MAX_WINDOWSTACK         (5)


/////////////////////////////////////////////////////////////////////////////////
// class Screen

class Screen
{
public:
    Screen();

    Window *    activateWindow(Window *wnd);
    Window *    switchWindow(Window *wnd);
    Window *    fallbackWindow(FallbackResult result);

    Window *    getActiveWindow();

protected:
    void        pushWindow(Window *wnd);
    Window *    popWindow();

protected:
    Window*     m_wndStack[MAX_WINDOWSTACK];
    int         m_topIndex;
};

#endif // __SCREEN_H__
