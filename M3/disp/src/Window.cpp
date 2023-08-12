// Window.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "Window.h"
#include "logger.h"


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

bool Window::setTimer(uint32_t id, uint32_t interval)
{
    // sanity check
    if (id == (uint32_t)-1)
        return false;

    for (size_t i = 0; i < sizeof(m_timer) / sizeof(m_timer[0]); i++)
    {
        if (m_timer[i].id == id)
            return false;
    }

    // find emtpy timer and active it
    for (size_t i = 0; i < sizeof(m_timer) / sizeof(m_timer[0]); i++)
    {
        if (m_timer[i].id == (uint32_t)-1)
        {
            LOGv("Activate Timer: %u %u", id, interval);
            m_timer[i].id = id;
            m_timer[i].interval = interval;
            m_timer[i].lastTick = millis();

            break;
        }
    }

    return false;
}

void Window::killTimer(uint32_t id)
{
    for (size_t i = 0; i < sizeof(m_timer) / sizeof(m_timer[0]); i++)
    {
        if (m_timer[i].id == id)
        {
            LOGv("Deactivate Timer: %u", id);
            m_timer[i].id = (uint32_t)-1;
            m_timer[i].interval = 0;
            m_timer[i].lastTick = 0;

            break;
        }
    }
}

void Window::onIdle()
{
    // check timer
    for (size_t i = 0; i < sizeof(m_timer) / sizeof(m_timer[0]); i++)
    {
        if (m_timer[i].id == (uint32_t)-1)
            continue;

        uint32_t elapse = millis() - m_timer[i].lastTick;
        if (elapse >= m_timer[i].interval)
        {
            this->onTimer(m_timer[i].id);

            // reset lastTick if it is still active
            if (m_timer[i].id != (uint32_t)-1)
                m_timer[i].lastTick = millis();
        }
    }

    // check drawing
    if (this->isDirty())
    {
        // call drawing callback
        this->onDraw(); 

        // now window validated
        this->setDirty(false);
    }
}