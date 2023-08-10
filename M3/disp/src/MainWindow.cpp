// MainWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "board.h"
#include "MainWindow.h"
#include "DeviceContext.h"




/////////////////////////////////////////////////////////////////////////////////
// class MainWindow

MainWindow::MainWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, LCD_WIDTH, LCD_HEIGHT)
    , m_ann(pRefCanvas)
    , m_thermal(pRefCanvas)
    , m_compass(pRefCanvas)
    , m_vmeter(pRefCanvas)
    , m_vbox { pRefCanvas, pRefCanvas, pRefCanvas, pRefCanvas, pRefCanvas, pRefCanvas }
    , m_widgets {
        &m_ann,
        &m_thermal,
        &m_compass,
        &m_vmeter,
        &m_vbox[0], &m_vbox[1], &m_vbox[2], &m_vbox[3], &m_vbox[4], &m_vbox[5]
    }
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
        m_widgets[i]->onDraw();

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

    /*
    m_vbox[0].setTitle("Altitude");
    m_vbox[0].setDescription("m");
    m_vbox[0].setValue(0, 0);

    m_vbox[1].setTitle("Speed H");
    m_vbox[1].setDescription("km/h");
    m_vbox[1].setValue(0, 0);

    m_vbox[2].setTitle("Speed V");
    m_vbox[2].setDescription("m/s");
    m_vbox[2].setValue(0, 2);

    //m_vbox[3].setTitle("Temp");
    //m_vbox[3].setDescription("Celsius");
    m_vbox[3].setTitle("Alt Baro");
    m_vbox[3].setDescription("m");
    m_vbox[3].setValue(0, 0);

    m_vbox[4].setTitle("Pressure");
    m_vbox[4].setDescription("hPa");
    m_vbox[4].setValue(0, 0);

    m_vbox[5].setTitle("Track");
    m_vbox[5].setDescription("deg");
    m_vbox[5].setValue(0, 0);
    */
}
