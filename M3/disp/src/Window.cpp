// Window.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "Window.h"



/////////////////////////////////////////////////////////////////////////////////
// class Window

//Window::Window()
//    : m_pRefCanvas(nullptr)
//    , m_x(0), m_y(0), m_w(0), m_h(0)
//    , m_flag(0)
//{
//}

Window::Window(M5EPD_Canvas *pRefCanvas) 
    : Widget(pRefCanvas)
{
}

Window::Window(M5EPD_Canvas *pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
{
}
