// MainWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "board.h"
#include "MainWindow.h"
#include "Message.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "logger.h"


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
        &m_ann,
        &m_thermal,
        &m_compass,
        &m_vmeter,
        &m_vbox[0], &m_vbox[1], &m_vbox[2], &m_vbox[3], &m_vbox[4], &m_vbox[5] }
    , m_tickLast(0)
    , m_tickShowMessage(0)
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

void MainWindow::draw()
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
    static uint32_t refreshCount = -1;
    m_pRefCanvas->pushCanvas(0, 0, ((++refreshCount) % 60) == 0 ? UPDATE_MODE_GC16 : UPDATE_MODE_DU);
}

void MainWindow::onActive()
{
    // layout widgets
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        switch (i)
        {
        case WID_ANNUNCIATOR:
            m_widgets[i]->move(0, 0, LCD_WIDTH, 60);
            m_widgets[i]->show(true);
            break;
        case WID_THERMALASSIST:
            m_widgets[i]->move(0, 60, 640, 360);
            m_widgets[i]->show(true);
            break;
        case WID_COMPASS:
            m_widgets[i]->move(0, 0, 0, 0);
            m_widgets[i]->show(false);
            break;
        case WID_VARIOMETER:
            m_widgets[i]->move(0, 0, 0, 0);
            m_widgets[i]->show(false);
            break;
        case WID_VALUEBOX_1:
            m_widgets[i]->move(640, 60, 320, 120);            
            m_widgets[i]->show(true);
            break;
        case WID_VALUEBOX_2:
            m_widgets[i]->move(640, 180, 320, 120);
            m_widgets[i]->show(true);
            break;
        case WID_VALUEBOX_3:
            m_widgets[i]->move(640, 300, 320, 120);
            m_widgets[i]->show(true);
            break;
        case WID_VALUEBOX_4:
            m_widgets[i]->move(0, 420, 320, 120);
            m_widgets[i]->show(true);
            break;
        case WID_VALUEBOX_5:
            m_widgets[i]->move(320, 420, 320, 120);
            m_widgets[i]->show(true);
            break;
        case WID_VALUEBOX_6:
            m_widgets[i]->move(640, 420, 320, 120);
            m_widgets[i]->show(true);
            break;
        }
    }

    #define MBOX_WIDTH      420
    #define MBOX_HEIGHT     80
    m_mbox.move((LCD_WIDTH - MBOX_WIDTH) / 2, (LCD_HEIGHT - MBOX_HEIGHT) / 2, MBOX_WIDTH, MBOX_HEIGHT);
    m_mbox.show(false);

    // initilize number-boxes
    uint16_t types[] = {
        ValueBox::VType::ALTITUDE_GROUND,
        ValueBox::VType::SPEED_GROUND,
        ValueBox::VType::SPEED_VERTICAL,
        ValueBox::VType::ALTITUDE_BARO,
        ValueBox::VType::ALTITUDE_AGL,
        ValueBox::VType::TRACK_HEADING
    };

    for (size_t i = 0; i < sizeof(m_vbox) / sizeof(m_vbox[0]); i++)
        m_vbox[i].init(types[i]);

}

void MainWindow::onMessage(uint16_t code, uint16_t data)
{
    DeviceContext *contextPtr = DeviceRepository::instance().getContext();

    switch (code)
    {
    case MSG_UPDATE_ANNUNCIATOR:
    case MSG_UPDATE_GPS:
    case MSG_UPDATE_VARIO:
    case MSG_UPDATE_BAT:
    case MSG_UPDATE_TH:
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

    case MSG_KEY_PRESSED:
    case MSG_KEY_LONG_PRESSED:
    case MSG_KEY_RELEASED:
        break;

    case MSG_TOUCH_PRESSED:
    case MSG_TOUCH_LONG_PRESSED:
    case MSG_TOUCH_RELEASED:
        break;
    }
}

void MainWindow::onTimer(uint32_t tickCount)
{    
    if (this->isDirty())
    {
        this->draw(); 
        this->setDirty(false);
    }

    if (m_mbox.isVisible() && (millis() - m_tickShowMessage) > 2000)
    {
        LOGv("Hide message box!");
        m_mbox.show(false);

        this->setDirty(true);
    }

    // 
    m_tickLast = tickCount;
}


void MainWindow::showMessage(const char *msg)
{
    LOGv("Show message box: %s", msg);
    m_mbox.setMessage(msg);
    m_mbox.show(true);
    m_tickShowMessage = millis();
    
    this->setDirty(true);
}
