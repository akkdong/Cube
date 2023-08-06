// Widgets.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "Widgets.h"
#include "DeviceContext.h"
#include "ImageResource.h"


/////////////////////////////////////////////////////////////////////////////////
// class Widget

//Widget::Widget()
//    : m_pRefCanvas(nullptr)
//    , m_x(0), m_y(0), m_w(0), m_h(0)
//    , m_flag(0)
//{
//}

Widget::Widget(M5EPD_Canvas *pRefCanvas)
    : m_pRefCanvas(pRefCanvas)
    , m_x(0), m_y(0), m_w(0), m_h(0)
    , m_flag(0){
}

Widget::Widget(M5EPD_Canvas *pRefCanvas, int x, int y, int w, int h)
    : m_pRefCanvas(pRefCanvas)
    , m_x(x), m_y(y), m_w(w), m_h(h)
    , m_flag(0)
{
}

void Widget::setDirty(bool dirty)
{
    if (dirty)
        m_flag = m_flag | WSTATE_UPDATED;
    else
        m_flag = m_flag & (~WSTATE_UPDATED);
}

void Widget::setPosition(int x, int y)
{
    m_x = x;
    m_y = y;

    m_flag = m_flag | WSTATE_UPDATED;
}

void Widget::move(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;

    m_flag = m_flag | WSTATE_UPDATED;
}

void Widget::show(bool show)
{
    if (show)
        m_flag = m_flag | WSTATE_VISIBLE;
    else
        m_flag = m_flag & (~WSTATE_VISIBLE);
}

int Widget::update(DeviceContext *context)
{
    return 0;
}

void Widget::onDraw()
{
}

void Widget::onTouchDown(int x, int y)
{
}

void Widget::onTouchMove(int x, int y)
{
}

void Widget::onTouchUp(int x, int y)
{
}

void Widget::onKeyPress(unsigned short key)
{
}

void Widget::onKeyLongPress(unsigned short key)
{
}

void Widget::onKeyRelease(unsigned short key)
{
}





/////////////////////////////////////////////////////////////////////////////////
// class Annunciator

Annunciator::Annunciator(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas)
{

}

Annunciator::Annunciator(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
{
}

int Annunciator::update(DeviceContext* context)
{
    //
    //
    //

    return 0;
}

void Annunciator::onDraw()
{
    m_pRefCanvas->drawRect(0, 0, M5EPD_PANEL_W, 60, M5EPD_Canvas::G15);
    int x = 8;
    int y = (60 - 32) / 2;
    m_pRefCanvas->pushImage(x, y, 32, 32, (const uint8_t *)ImageResource_logo_wolf_howl_32x32);
    x += 32 + 8;
    y = (60 - 32) / 2;
    m_pRefCanvas->pushImage(x, y, 32, 32, (const uint8_t *)ImageResource_bluetooh_paired_32x32);
    x += 32 + 4;
    m_pRefCanvas->pushImage(x, y, 32, 32, (const uint8_t *)ImageResource_gps_fixed_32x32);
    x += 32 + 4;
    m_pRefCanvas->pushImage(x, y, 32, 32, (const uint8_t *)ImageResource_sd_logging_32x32);
    x += 32 + 4;
    m_pRefCanvas->pushImage(x, y, 32, 32, (const uint8_t *)ImageResource_volume_on_32x32);
    x += 32 + 4;
    m_pRefCanvas->pushImage(x, y, 32, 32, (const uint8_t *)ImageResource_battery_full_32x32);

    // time
    char sz[32];
    getTimeString(sz, time(NULL), true);

    m_pRefCanvas->setTextSize(32);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);      
    m_pRefCanvas->setTextDatum(CR_DATUM);
    m_pRefCanvas->drawString(sz, M5EPD_PANEL_W - 8, 30);

}

const char * Annunciator::getTimeString(char* str, time_t t, bool includeSecond)
{
	struct tm * _tm = localtime(&t);    
    if (includeSecond)
	    sprintf(str, "%02d:%02d:%02d", _tm->tm_hour, _tm->tm_min,_tm->tm_sec);
    else
        sprintf(str, "%02d:%02d", _tm->tm_hour, _tm->tm_min);

    return str;
}




/////////////////////////////////////////////////////////////////////////////////
// class ValueBox

int ValueBox::m_titleFontSize = 32;
int ValueBox::m_valueFontSize = 70;

ValueBox::ValueBox(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas, 0, 0, 0, 0)
    , m_title("")
    , m_desc("")
    , m_value(0)
    , m_decimalPlace(1)    
{

}

ValueBox::ValueBox(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
    , m_title("")
    , m_desc("")
    , m_value(0)
    , m_decimalPlace(1)    
{

}

void ValueBox::setFontSize(int titleSize, int valueSize)
{
    m_titleFontSize = titleSize;
    m_valueFontSize = valueSize;
}

int ValueBox::update(DeviceContext* context)
{
    //
    // update value and title, desc if necessary
    //

    return 0;
}

void ValueBox::onDraw()
{
    m_pRefCanvas->fillRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G0);
    m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);

    m_pRefCanvas->setTextDatum(TL_DATUM);
    m_pRefCanvas->setTextSize(m_titleFontSize);
    m_pRefCanvas->drawString(m_title, m_x + 8, m_y + 4);
    m_pRefCanvas->setTextDatum(TR_DATUM);
    m_pRefCanvas->drawString(m_desc, m_x + m_w - 8, m_y + 4);
    m_pRefCanvas->setTextDatum(BR_DATUM);
    m_pRefCanvas->setTextSize(m_valueFontSize);

    char sz[32];
    sprintf(sz, "%.*f", m_decimalPlace, m_value);
    m_pRefCanvas->drawString(sz, m_x + m_w - 8, m_y + m_h);
}



/////////////////////////////////////////////////////////////////////////////////
// class ThermalAssist

ThermalAssist::ThermalAssist(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas)
{

}

ThermalAssist::ThermalAssist(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
{
}

int ThermalAssist::update(DeviceContext* context)
{
    return 0;
}

void ThermalAssist::onDraw()
{

}






/////////////////////////////////////////////////////////////////////////////////
// class Compass

Compass::Compass(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas)
{

}

Compass::Compass(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
{
}

int Compass::update(DeviceContext* context)
{
    return 0;
}

void Compass::onDraw()
{

}






/////////////////////////////////////////////////////////////////////////////////
// class VarioMeter

VarioMeter::VarioMeter(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas)
{

}

VarioMeter::VarioMeter(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
{
}

int VarioMeter::update(DeviceContext* context)
{
    return 0;
}

void VarioMeter::onDraw()
{

}

