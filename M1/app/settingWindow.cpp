// settingWindow.cpp
//

#include <stdint.h>
#include "device_defines.h"
#include "settingWindow.h"


///////////////////////////////////////////////////////////////////////////////////
// class SettingWindow


SettingWindow::SettingWindow()
{

}

SettingWindow::~SettingWindow()
{

}

void SettingWindow::update()
{

}

//
void SettingWindow::onCreate()
{

}

void SettingWindow::onActive()
{

}

void SettingWindow::onClose()
{

}

void SettingWindow::onKeyDown(uint16_t key)
{
    if (key == KEY_ENTER)
    {
        Screen::instance()->fallbackWindow(Window::RES_OK);
    }
}

void SettingWindow::onKeyLongDown(uint16_t key)
{

}

void SettingWindow::onKeyUp(uint16_t key)
{

}
