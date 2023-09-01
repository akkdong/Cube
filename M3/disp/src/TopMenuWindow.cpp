// topMenuWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "board.h"
#include "TopMenuWindow.h"
#include "Message.h"
#include "Application.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "ImageResource.h"
#include "logger.h"


#define MAX_BUTTON      (4)
#define IMAGE_W         (128)
#define IMAGE_H         (128)
#define LABEL_H         (48)
#define BUTTON_W        (IMAGE_W + 64)
#define BUTTON_H        (IMAGE_H + LABEL_H + 42)

#define BUTTON_SPACE    (int)((LCD_WIDTH - (BUTTON_W * MAX_BUTTON)) / (MAX_BUTTON * 2 - 1))
#define BUTTON_SX       (BUTTON_SPACE + BUTTON_SPACE)
#define BUTTON_SY       (LCD_HEIGHT - (BUTTON_H + BUTTON_SPACE + BUTTON_SPACE + BUTTON_SPACE))



/////////////////////////////////////////////////////////////////////////////////
// class TopMenuWindow
//
//  +------------------------------------------------+
//  | Logo: Title                                    |
//  |                                                |
//  |                                                |
//  |   +-------+  +-------+  +-------+  +-------+   |
//  |   |       |  |       |  |       |  |       |   |
//  |   |       |  |       |  |       |  |       |   |
//  |   +-------+  +-------+  +-------+  +-------+   |
//  |     xxxx       xxxx       xxxx       xxxx      |
//  |                                                |
//  +------------------------------------------------+

TopMenuWindow::TopMenuWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, LCD_WIDTH, LCD_HEIGHT)
    , m_btn {
        { pRefCanvas, 0, 0, 0, 0 },
        { pRefCanvas, 0, 0, 0, 0 },
        { pRefCanvas, 0, 0, 0, 0 },
        { pRefCanvas, 0, 0, 0, 0 },
    }
    , m_layout {
        { 
            BUTTON_SX + (BUTTON_W + BUTTON_SPACE) * 0, BUTTON_SY, BUTTON_W, BUTTON_H, 
            ImageResource_icon_wifi_128x128, IMAGE_W, IMAGE_H,
            " WiFi "
        },
        { 
            BUTTON_SX + (BUTTON_W + BUTTON_SPACE) * 1, BUTTON_SY, BUTTON_W, BUTTON_H, 
            ImageResource_icon_route_128x128, IMAGE_W, IMAGE_H,
            " Route "
        },
        { 
            BUTTON_SX + (BUTTON_W + BUTTON_SPACE) * 2, BUTTON_SY, BUTTON_W, BUTTON_H, 
            ImageResource_icon_setting_128x128, IMAGE_W, IMAGE_H,
            " Setting "
        },
        { 
            BUTTON_SX + (BUTTON_W + BUTTON_SPACE) * 3, BUTTON_SY, BUTTON_W, BUTTON_H, 
            ImageResource_icon_power_128x128, IMAGE_W, IMAGE_H,
            " Power "
        },
    }
    , m_lastKey(0)
    , m_active(-1)
    , m_lastTouch(0)
    , m_drawCount(0)
{
}


void TopMenuWindow::onActive()
{
    // select first button
    m_active = 0;

    for (size_t i = 0; i < MENU_COUNT; i++)
    {
        LayoutInfo *layout = &m_layout[i];
        ButtonClass *btn = &m_btn[i];

        btn->setImage(layout->image, layout->iw, layout->ih);
        btn->setLabel(layout->label);
        btn->setTextSize(LABEL_H);
        btn->setActive(i == m_active);
        btn->init(0);
        btn->move(layout->x, layout->y, layout->w, layout->h);
        btn->show(true);
    }

    redraw();
}

void TopMenuWindow::onDraw()
{
    // erase bkgnd
    m_pRefCanvas->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, M5EPD_Canvas::G0);

    for (size_t i = 0; i < MENU_COUNT; i++)
    {
        if (m_btn[i].isVisible())
            m_btn[i].draw();
    }

    int x = BUTTON_SPACE;
    int y = BUTTON_SPACE;
    m_pRefCanvas->pushImage(x, y, 48, 48, ImageResource_logo_wolf_small_48x48);
    m_pRefCanvas->setTextDatum(ML_DATUM);
    m_pRefCanvas->setTextSize(LABEL_H);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
    m_pRefCanvas->drawString("Cube M3 Flight computer", x + 48 + 4, y + 48 / 2);    

    // update full or fast(?)
    m_pRefCanvas->pushCanvas(0, 0, m_drawCount == 0 ? UPDATE_MODE_GC16 : UPDATE_MODE_DU);
    m_drawCount += 1;
}

void TopMenuWindow::onMessage(uint32_t code, uint32_t data)
{
    switch (code)
    {
    case MSG_INVALIDATE:
        if (data) redraw(); else redrawFast();
        break;
    }
}

void TopMenuWindow::onTimer(uint32_t id)
{
}

void TopMenuWindow::onTouchDown(int x, int y)
{
    int index = hitTest(x, y);
    LOGv("TopMenu hit-test: (%d, %d) -> %d", x, y, index);
    if (index >= 0 && index < MENU_COUNT)
    {
        LOGv("touch button: %d", index);
        activateButton(m_active, false);
        bool updated = m_active != index;
        m_active = index;
        activateButton(m_active, true);

        if (updated)
            Application::getApp()->sendMessage(MSG_INVALIDATE);

        m_lastTouch = millis();
    }
}

void TopMenuWindow::onTouchMove(int x, int y)
{
}

void TopMenuWindow::onTouchUp(int x, int y)
{
    int index = hitTest(x, y);
    if (index >= 0 && m_active == index)
    {
        if (millis() - m_lastTouch > 2000)
            exec(m_active);
    }
}

void TopMenuWindow::onKeyPressed(uint32_t key)
{
    m_lastKey = key;
}

void TopMenuWindow::onKeyLongPressed(uint32_t key)
{
    switch (key)
    {
    case EXT_KEY_LEFT:
    case EXT_KEY_RIGHT:
        // nop
        break;
    case EXT_KEY_UP:
        if (m_active >= 0)
            exec(m_active);
        break;
    case EXT_KEY_DOWN:
        Application::getApp()->sendMessage(MSG_FALLBACK);
        break;
    }

    m_lastKey = 0;
}

void TopMenuWindow::onKeyReleased(uint32_t key)
{
    if (m_lastKey == 0)
        return;

    switch (key)
    {
    case KEY_LEFT:
    case EXT_KEY_LEFT:
        if (moveSelection(-1))
            Application::getApp()->sendMessage(MSG_INVALIDATE);
        break;
    case KEY_RIGHT:
    case EXT_KEY_RIGHT:
        if (moveSelection(1))
            Application::getApp()->sendMessage(MSG_INVALIDATE);
        break;
    case EXT_KEY_UP:
    case EXT_KEY_DOWN:
        // nop
        break;
    }
}

int TopMenuWindow::hitTest(int x, int y)
{
    for (int i = 0; i < MENU_COUNT; i++)
    {
        if (m_btn[i].pointInWidget(x, y))
            return i;
    }

    return -1;
}

void TopMenuWindow::activateButton(int index, bool active)
{
    if (index >= 0 && index < MENU_COUNT)
        m_btn[index].setActive(active);
}

bool TopMenuWindow::moveSelection(int offset)
{
    int next = m_active;
    if (next < 0)
        next = 0;
    else
        next += offset;

    if (next < 0)
        next = MENU_COUNT - 1;
    if (next >= MENU_COUNT)
        next = 0;

    if (next == m_active)
        return false;

    LOGv("select button: %d", next);
    activateButton(m_active, false);
    m_active = next;
    activateButton(m_active, true);

    return true;
}

void TopMenuWindow::exec(int index)
{
    // function is selected! : invoke launch-command
    LOGv("Execute #%d command", index);
    switch (index)
    {
    case 0:
        Application::getApp()->sendMessage(MSG_SHOW_WIFI);
        break;
    case 1:
        Application::getApp()->sendMessage(MSG_SHOW_ROUTE);
        break;
    case 2:
        Application::getApp()->sendMessage(MSG_SHOW_SETTINGS);
        break;
    case 3:
        Application::getApp()->sendMessage(MSG_PRE_SHUTDOWN);
        break;
    }
}

void TopMenuWindow::redraw()
{
    // reset draw-count
    m_drawCount = 0;
    // draw now!!
    draw();
}

void TopMenuWindow::redrawFast()
{
    draw();
}
