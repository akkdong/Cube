// Screen.cpp
//

#include <Arduino.h>
#include "Window.h"
#include "Screen.h"

/////////////////////////////////////////////////////////////////////////////////
// class Screen

Screen::Screen() 
{
    for (size_t i = 0; i < sizeof(m_wndStack) / sizeof(m_wndStack[0]); i++)
        m_wndStack[i] = nullptr;

    m_topIndex = -1;
}

Window * Screen::activateWindow(Window *wnd)
{
    if (m_topIndex + 1 == sizeof(m_wndStack) / sizeof(m_wndStack[0]))
        return nullptr;

    // push window to top
    m_wndStack[++m_topIndex] = wnd;
    // notify activation
    wnd->onActive();

    return wnd;
}

Window * Screen::switchWindow(Window *wnd)
{
    return nullptr;
}

Window * Screen::fallbackWindow(FallbackResult result)
{
    if (m_topIndex < 1)
        return nullptr;

    // pop top-most window 
    Window *top = popWindow();
    // and notify close
    top->onClose();

    // get below(active) window
    Window *active = getActiveWindow();
    // and notify fallback
    active->onFallback(result, top);
    // remove fast top-most window
    delete top;

    return active;
;
}


Window * Screen::getActiveWindow() 
{
    if (m_topIndex < 0)
        return nullptr;

    return m_wndStack[m_topIndex];
}    

void Screen::pushWindow(Window *wnd)
{
    m_wndStack[++m_topIndex] = wnd;
}

Window * Screen::popWindow()
{
    return m_wndStack[m_topIndex--];
}
