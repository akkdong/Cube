// MainWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "board.h"
#include "MainWindow.h"
#include "Message.h"
#include "Application.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "logger.h"

#define TIMERID_HIDE_MESSAGEBOX     (1)



/////////////////////////////////////////////////////////////////////////////////
// class MainWindow

MainWindow::MainWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, LCD_WIDTH, LCD_HEIGHT)
    , m_ann(pRefCanvas)
    , m_thermal(pRefCanvas)
    , m_compass(pRefCanvas)
    , m_vmeter(pRefCanvas)
    , m_vbox { pRefCanvas, pRefCanvas, pRefCanvas, pRefCanvas, pRefCanvas, pRefCanvas }
    , m_mbox (pRefCanvas)
    , m_widgets {
        &m_ann, &m_thermal, &m_compass, &m_vmeter,
        &m_vbox[0], &m_vbox[1], &m_vbox[2], &m_vbox[3], &m_vbox[4], &m_vbox[5] 
    }
    , m_layout {
        {   { 
                { 0, 0, 960, 60, true, 0 }, // WID_ANNUNCIATOR
                { 0, 60, 640, 360, true, 0 }, // WID_THERMALASSIST
                { 0, 0, 0, 0, false, 0 }, // WID_COMPASS
                { 0, 0, 0, 0, false, 0 }, // WID_VARIOMETER
                { 640, 60, 320, 120, true, ValueBox::ALTITUDE_GROUND | ValueBox::BORDER_LEFT|ValueBox::BORDER_BOTTOM|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_1
                { 640, 180, 320, 120, true, ValueBox::SPEED_GROUND | ValueBox::BORDER_LEFT|ValueBox::BORDER_BOTTOM|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_2
                { 640, 300, 320, 120, true, ValueBox::SPEED_VERTICAL | ValueBox::BORDER_LEFT|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_3
                { 0, 420, 320, 120, true, ValueBox::ALTITUDE_BARO | ValueBox::BORDER_RIGHT|ValueBox::BORDER_TOP|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_4
                { 320, 420, 320, 120, true, ValueBox::ALTITUDE_AGL | ValueBox::BORDER_RIGHT|ValueBox::BORDER_TOP|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_5
                { 640, 420, 320, 120, true, ValueBox::TRACK_HEADING | ValueBox::BORDER_TOP|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_6
            },
            32, 76
        },
        {
            {
                { 0, 0, 960, 60, true, 0 }, // WID_ANNUNCIATOR
                { 0, 0, 0, 0, false, 0 }, // WID_THERMALASSIST
                { 380, 60, 200, 200, true, 0 }, // WID_COMPASS
                { 400, 260, 160, 240, true, 0 }, // WID_VARIOMETER
                { 0, 60, 380, 160, true, ValueBox::ALTITUDE_GROUND | ValueBox::BORDER_RIGHT|ValueBox::BORDER_BOTTOM|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_1
                { 0, 220, 380, 160, true, ValueBox::SPEED_GROUND | ValueBox::BORDER_RIGHT|ValueBox::BORDER_BOTTOM|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_2
                { 0, 380, 380, 160, true, ValueBox::TRACK_HEADING | ValueBox::BORDER_RIGHT|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_3
                { 580, 60, 380, 160, true, ValueBox::SPEED_VERTICAL | ValueBox::BORDER_LEFT|ValueBox::BORDER_BOTTOM|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_4
                { 580, 220, 380, 160, true, ValueBox::GLIDE_RATIO | ValueBox::BORDER_LEFT|ValueBox::BORDER_BOTTOM|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_5
                { 580, 380, 380, 160, true, ValueBox::DISTANCE_TAKEOFF | ValueBox::BORDER_LEFT|ValueBox::ALIGN_LABEL_TL|ValueBox::ALIGN_UNIT_TR|ValueBox::ALIGN_VALUE_BR }, // WID_VALUEBOX_6
            },
            32, 92
        },
        {
            {
                { 0, 0, 960, 60, true, 0 }, // WID_ANNUNCIATOR
                { 0, 60, 960, 480, true, 0 }, // WID_THERMALASSIST
                { 0, 0, 0, 0, false, 0 }, // WID_COMPASS
                { 0, 0, 0, 0, false, 0 }, // WID_VARIOMETER
                { 640, 60, 320, 120, true, ValueBox::ALTITUDE_GROUND |  ValueBox::ALIGN_UNIT_BR|ValueBox::ALIGN_VALUE_TR}, // WID_VALUEBOX_1
                { 640, 180, 320, 120, true, ValueBox::SPEED_GROUND | ValueBox::ALIGN_UNIT_BR|ValueBox::ALIGN_VALUE_TR }, // WID_VALUEBOX_2
                { 640, 420, 320, 120, true, ValueBox::SPEED_VERTICAL | ValueBox::ALIGN_UNIT_BR|ValueBox::ALIGN_VALUE_TR }, // WID_VALUEBOX_3
                { 0, 0, 0, 0, false, 0 }, // WID_VALUEBOX_4
                { 0, 0, 0, 0, false, 0 }, // WID_VALUEBOX_5
                { 0, 0, 0, 0, false, 0 }, // WID_VALUEBOX_6
            },
            32, 76
        },
    }
    , m_activePage(-1)
    , m_lastKey(0)
    , m_refreshCount(-1)
{
}

int MainWindow::update(DeviceContext *context, uint32_t updateHints) 
{
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        int update = m_widgets[i]->update(context, updateHints);
        if (update > 0)
            this->setDirty(true);
    }

    return isDirty() ? 1 : 0;
}

void MainWindow::onDraw()
{
    // erase bkgnd
    m_pRefCanvas->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, M5EPD_Canvas::G0);

    // draw each widgets 
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        if (m_widgets[i]->isVisible())
            m_widgets[i]->draw();
    }

    // draw notify mesage
    if (m_mbox.isVisible())
        m_mbox.draw();

    // update full or fast(?)
    m_pRefCanvas->pushCanvas(0, 0, ((++m_refreshCount) % 60) == 0 ? UPDATE_MODE_GC16 : UPDATE_MODE_DU);
}

void MainWindow::onActive()
{
    changePage(1);

    #define MBOX_WIDTH      420
    #define MBOX_HEIGHT     80
    m_mbox.move((LCD_WIDTH - MBOX_WIDTH) / 2, (LCD_HEIGHT - MBOX_HEIGHT) / 2, MBOX_WIDTH, MBOX_HEIGHT);
    m_mbox.show(false);
}

void MainWindow::onMessage(uint32_t code, uint32_t data)
{
    DeviceContext *contextPtr = DeviceRepository::instance().getContext();

    switch (code)
    {
    //case MSG_UPDATE_ANNUNCIATOR:
    case MSG_UPDATE_GPS:
    case MSG_UPDATE_VARIO:
    //case MSG_UPDATE_BAT:
    //case MSG_UPDATE_TH:
        this->update(contextPtr, /*updateHints*/ 0);
        break;

    case MSG_GPS_FIXED:
        // update annunciator
        //isUpdated = active->update(contextPtr, /*updateHints*/ 0);
        // & show message
        this->showMessage(data > 0 ? "GPS Fixed!" : "GPS Unfixed!");
        break;

    case MSG_TAKEOFF:
        this->showMessage("Takeoff...");
        break;
    case MSG_LANDING:
        this->showMessage("Landing...");
        break;

    /*
    case MSG_KEY_PRESSED:
        onKeyPressed(data);
        break;
    case MSG_KEY_LONG_PRESSED:
        onKeyLongPressed(data);
        break;
    case MSG_KEY_RELEASED:
        onKeyReleased(data);
        break;

    case MSG_TOUCH_DOWN:
        onTouchDown(data >> 16, data & 0x00FF);
        break;
    case MSG_TOUCH_MOVE:
        onTouchMove(data >> 16, data & 0x00FF);
        break;
    case MSG_TOUCH_UP:
        onTouchUp(data >> 16, data & 0x00FF);
        break;
    */
    }
}

void MainWindow::onTimer(uint32_t id)
{
    LOGv("MainWindow::onTimer(%u)", id);
    if (id == TIMERID_HIDE_MESSAGEBOX)
    {
        LOGv("Hide message box!");
        m_mbox.show(false);
        this->setDirty(true);

        killTimer(id);
    }
}

void MainWindow::onTouchDown(int x, int y)
{
    LOGv("onTouchDown: %d, %d", x, y);
}

void MainWindow::onTouchMove(int x, int y)
{
    LOGv("onTouchMove: %d, %d", x, y);
}

void MainWindow::onTouchUp(int x, int y)
{
    LOGv("onTouchUp: %d, %d", x, y);
}

void MainWindow::onKeyPressed(uint32_t key)
{
    LOGv("onKeyPressed: %u", key);

    // save latest key
    m_lastKey = key;
}

void MainWindow::onKeyLongPressed(uint32_t key)
{
    LOGv("onKeyLongPressed: %u", key);

    if (key == KEY_LEFT)
    {
        // Toggle sound
        //

        // send mute-command
        DeviceContext *contextPtr = DeviceRepository::instance().getContext();
        String muteCmd("MUTE x");

        if (contextPtr->volume.vario > 0)
            muteCmd[5] = '1';
        else
            muteCmd[5] = '0';

        Serial1.println(muteCmd.c_str());

        // show command
        this->showMessage(muteCmd.c_str());

        // ignore key-left
        m_lastKey = 0;
    }
    else if (key == KEY_RIGHT)
    {
        // Toggle BT
        //
        // ....
    }
    else if (key == EXT_KEY_LEFT)
    {
        // show top-menu
        Application::getApp()->sendMessage(MSG_SHOW_TOPMENU, 0);

        // ignore key-up
        m_lastKey = 0;
    }
    else if (key == EXT_KEY_RIGHT)
    {
        // show statistic
        Application::getApp()->sendMessage(MSG_SHOW_STATISTIC, 0);

        // ignore key-up
        m_lastKey = 0;
    }
    else if (key == EXT_KEY_UP)
    {
        // ENTER
        // ...
    }
    else if (key == EXT_KEY_DOWN)
    {
        // ESC (BACK)
        // ...

        /*
        // toggle simulation mode
        bool enable = Application::getApp()->isEngineerMode() ? false : true;
        Application::getApp()->enableEngineerMode(enable);

        // show command
        String str("EngMode x");
        str[8] = enable ? '1' : '0';
        this->showMessage(str.c_str());
        */
    }
}

void MainWindow::onKeyReleased(uint32_t key)
{
    LOGv("onKeyReleased: %u", key);
    if (m_lastKey != key)
        return;

    switch (key)
    {
    case KEY_LEFT:
    case EXT_KEY_LEFT:
        // prev page
        changePage(m_activePage - 1);
        break;
    case KEY_PUSH:
        break;
    case EXT_KEY_RIGHT:
    case KEY_RIGHT:
        // next page
        changePage(m_activePage + 1);
        break;
    case EXT_KEY_UP:
        // prev/up item
        break;
    case EXT_KEY_DOWN:
        // next/down item
        break;
    }
}


void MainWindow::showMessage(const char *msg)
{
    LOGv("Show message box: %s", msg);
    m_mbox.setMessage(msg);
    m_mbox.show(true);
    this->setDirty(true);

    // hide after 2 seconds
    setTimer(TIMERID_HIDE_MESSAGEBOX, 2000);
}

void MainWindow::changePage(int pageNum)
{
    if (pageNum < 0)
        pageNum = PID_COUNT - 1;
    else if (pageNum >= PID_COUNT)
        pageNum = 0;

    if (m_activePage == pageNum)
        return;
    m_activePage = pageNum;

    // layout each widget
    Layout *layout = &m_layout[pageNum];
    LOGv("Layout Page #%d", pageNum);
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        LayoutInfo *widget = &layout->widget[i];
        LOGv(" Widget[%d]: %d, %d, %d, %d, %d, %x",
            i, widget->x, widget->y, widget->w, widget->h, widget->visible, widget->flag);

        m_widgets[i]->init(widget->flag);
        m_widgets[i]->move(widget->x, widget->y, widget->w, widget->h);
        m_widgets[i]->show(widget->visible);
    }
    // change ValueBox font size
    ValueBox::setFontSize(layout->vbTitleSize, layout->vbValueSize);

    // redraw all    
    this->setDirty(true);
    // refresh with no ghost
    m_refreshCount = -1;
}