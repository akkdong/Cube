// Window.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "bsp.h"
#include "Window.h"
#include "DeviceContext.h"



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





/////////////////////////////////////////////////////////////////////////////////
// class MainWindow

MainWindow::MainWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, M5EPD_PANEL_W, M5EPD_PANEL_H)
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
    /*
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        switch (i)
        {
        case WID_ANNUNCIATOR:
            m_widgets[i] = &m_ann;
            break;
        case WID_THERMALASSIST:
            m_widgets[i] = &m_thermal;
            break;
        case WID_COMPASS:
            m_widgets[i] = new Compass(pRefCanvas);
            break;
        case WID_VARIOMETER:
            m_widgets[i] = new VarioMeter(pRefCanvas);
            break;
        case WID_VALUEBOX_1:
        case WID_VALUEBOX_2:
        case WID_VALUEBOX_3:
        case WID_VALUEBOX_4:
        case WID_VALUEBOX_5:
        case WID_VALUEBOX_6:
            m_widgets[i] = new ValueBox(pRefCanvas);
        }
    }
    */
}

int MainWindow::update(DeviceContext *context) 
{
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        int update = m_widgets[i]->update(context);
        if (update > 0)
            this->setDirty(true);
    }

    return isDirty() ? 1 : 0;
}

void MainWindow::draw()
{
    // erase bkgnd
    m_pRefCanvas->fillRect(0, 0, M5EPD_PANEL_W, M5EPD_PANEL_H, M5EPD_Canvas::G0);

    // draw each widgets 
    for (size_t i = 0; i < WID_COUNT; i++)
        m_widgets[i]->onDraw();

    // update full or fast(?)
    m_pRefCanvas->pushCanvas(0, 0, UPDATE_MODE_GLR16);
}

void MainWindow::onActive()
{
    // layout widgets
    for (size_t i = 0; i < WID_COUNT; i++)
    {
        switch (i)
        {
        case WID_ANNUNCIATOR:
            m_widgets[i]->move(0, 0, LCD_WIDTH, LCD_HEIGHT);
            break;
        case WID_THERMALASSIST:
            m_widgets[i]->move(0, 60, 640, 360);
            break;
        case WID_COMPASS:
            m_widgets[i]->move(0, 0, 0, 0);
            break;
        case WID_VARIOMETER:
            m_widgets[i]->move(0, 0, 0, 0);
            break;
        case WID_VALUEBOX_1:
            m_widgets[i]->move(640, 60, 320, 120);            
            break;
        case WID_VALUEBOX_2:
            m_widgets[i]->move(640, 180, 320, 120);
            break;
        case WID_VALUEBOX_3:
            m_widgets[i]->move(640, 300, 320, 120);
            break;
        case WID_VALUEBOX_4:
            m_widgets[i]->move(0, 420, 320, 120);
            break;
        case WID_VALUEBOX_5:
            m_widgets[i]->move(320, 420, 320, 120);
            break;
        case WID_VALUEBOX_6:
            m_widgets[i]->move(640, 420, 320, 120);
            break;
        }        
    }

    m_vbox[0].setTitle("Altitude");
    m_vbox[0].setDescription("m");
    m_vbox[0].setValue(2045, 0);

    m_vbox[1].setTitle("Speed H");
    m_vbox[1].setDescription("km/h");
    m_vbox[1].setValue(32.4, 1);

    m_vbox[2].setTitle("Speed V");
    m_vbox[2].setDescription("m/s");
    m_vbox[2].setValue(2.54, 2);

    m_vbox[3].setTitle("AGL");
    m_vbox[3].setDescription("m");
    m_vbox[3].setValue(1045, 0);

    m_vbox[4].setTitle("L/D");
    m_vbox[4].setDescription("");
    m_vbox[4].setValue(2.4, 1);

    m_vbox[5].setTitle("Track");
    m_vbox[5].setDescription("deg.");
    m_vbox[5].setValue(245, 0);
}
