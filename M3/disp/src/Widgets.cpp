// Widgets.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include "Widgets.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "ImageResource.h"
#include "utils.h"


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
    // 109876543210           
    // BBGGSSVVCCCC
    // BT
    //   GPS
    //     SD
    //       VOLUME
    //         BATTERY
    mState = 0;
    mState |= (uint32_t)context->deviceState.statusBT << 10;
    mState |= (uint32_t)context->deviceState.statusGPS << 8;
    mState |= (uint32_t)context->deviceState.statusSDCard << 6;
    mState |= (uint32_t)(context->volume.vario > 0 ? 1 : 0) << 4;
    
    uint32_t bat = 5; // empty, 1/4, 2/4, 3/4, 4/4, charging
    if (context->deviceState.batteryPower < 3.3)
    {
        bat = 4;
        if (context->deviceState.batteryPower < 3.1)
        {
            bat = 3;
            if (context->deviceState.batteryPower < 3.0)
            {
                bat = 2;
                if (context->deviceState.batteryPower < 2.9)
                {
                    bat = 1;
                    if (context->deviceState.batteryPower < 2.8)
                        bat = 0;
                }
            }
        }
    }
    mState |= bat;
    mVoltage = context->deviceState.batteryPower;

    return 1;
}

void Annunciator::onDraw()
{
    #define ICON_W      32
    #define ICON_H      32
    #define STATUS_H    60
    #define ICON_MARGIN  8
    #define ICON_SPACE   4

    //m_pRefCanvas->drawRect(m_x, m_y, M5EPD_PANEL_W, STATUS_H, M5EPD_Canvas::G15);
    m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);
    int x = m_x + 8;
    int y = m_y + (m_h - ICON_H) / 2;
    // WOLF
    {
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, (const uint8_t *)ImageResource_logo_wolf_howl_32x32);
        x += ICON_W + ICON_MARGIN;
    }
    // BT
    uint8_t state = (mState >> 10) & 0x03;
    if( state > 0)
    {
        const uint8_t *img = state > 1 ? ImageResource_bluetooh_paired_32x32 : ImageResource_bluetooth_unpaired_32x32;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // GPS
    state = (mState >> 8) & 0x03;
    {
        const uint8_t *img = state > 0 ? ImageResource_gps_fixed_32x32 : ImageResource_gps_unfixed_32x32;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // SD
    state = (mState >> 6) & 0x03;
    {
        const uint8_t *img = state > 1 ? ImageResource_sd_logging_32x32 : ImageResource_sd_ready_32x32;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // VOLUME
    state = (mState >> 4) & 0x03;
    {
        const uint8_t *img = state > 0 ? ImageResource_volume_on_32x32 : ImageResource_volume_mute_32x32;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // BATTERY
    state = mState & 0x0F;
    {
        const uint8_t *img;
        switch (state)
        {
        case 5: img = ImageResource_battery_charging_32x32; break;
        case 4: img = ImageResource_battery_full_32x32; break;
        case 3: img = ImageResource_battery_three_quaters_32x32; break;
        case 2: img = ImageResource_battery_half_32x32; break;
        case 1: img = ImageResource_battery_one_quaters_32x32; break;
        case 0:
        default:  img = ImageResource_battery_empty_32x32; break;
        }

        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, (const uint8_t *)ImageResource_battery_full_32x32);
        x += ICON_W + ICON_SPACE;
        y = m_y + m_h / 2;

        char sz[32];
        sprintf(sz, "%.2f", mVoltage);
        m_pRefCanvas->setTextSize(32);
        m_pRefCanvas->setTextDatum(CL_DATUM);
        m_pRefCanvas->drawString(sz, x, y);
    }

    // time
    {
        char sz[32];
        getTimeString(sz, time(NULL), true);

        m_pRefCanvas->setTextSize(32);
        m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
        m_pRefCanvas->setTextDatum(CR_DATUM);
        m_pRefCanvas->drawString(sz, m_x + m_w - ICON_MARGIN, m_y + m_h / 2);
    }
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
    // update value and title, desc if necessary
    switch (getUserData())
    {
    case ValueBox::VType::ALTITUDE_GROUND:
        setValue(context->varioState.altitudeGPS, 0);
        setDirty(true);
        break;
    case ValueBox::VType::ALTITUDE_BARO:
        setValue(context->varioState.altitudeBaro, 0);
        setDirty(true);
        break;
    case ValueBox::VType::SPEED_GROUND:
        setValue(context->varioState.speedGround, 0);
        setDirty(true);
        break;
    case ValueBox::VType::SPEED_VERTICAL:
        setValue(context->varioState.speedVertActive, 2);
        setDirty(true);
        break;
    case ValueBox::VType::SENSOR_TEMPERATURE:
        setValue(context->varioState.temperature, 0);
        setDirty(true);
        break;
    case ValueBox::VType::SENSOR_PRESSURE:
        setValue(context->varioState.pressure, 0);
        setDirty(true);
        break;
    case ValueBox::VType::TRACK_HEADING:
        setValue(context->varioState.heading, 0);
        setDirty(true);
        break;
    default:
        break;
    }

    return isDirty() ? 1 : 0;
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


#define ZOOM_FACTOR				(1.0)

void ThermalAssist::onDraw()
{
    DeviceContext *context = DeviceRepository::instance().getContext();

    //
	float theta = TO_RADIAN(180 - context->varioState.heading);

	for (int i = context->flightState.rearPoint; i != context->flightState.frontPoint; )
	{
#if 1 // 0 clock is heading
		float x0 = context->flightState.trackDistance[i].dx * ZOOM_FACTOR;
		float y0 = context->flightState.trackDistance[i].dy * ZOOM_FACTOR;

		int16_t x = (m_x + m_w / 2) - (int16_t)(x0 * cos(theta) - y0 * sin(theta));
		int16_t y = (m_y + m_h / 2) - (int16_t)(x0 * sin(theta) + y0 * cos(theta));
#else // 0 clock is north
		int16_t x = m_x + m_w / 2;
		int16_t y = m_y + m_h / 2;

		x -= context->flightState.trackDistance[i].dx * ZOOM_FACTOR;
		y += context->flightState.trackDistance[i].dy * ZOOM_FACTOR;
#endif

		if (m_x + 2 < x && m_y + 2 < y && x < m_x + m_w - 2 && y < m_y + m_h - 2)
		{
			if (context->flightState.trackPoints[i].vario < 0)
			{
				m_pRefCanvas->drawRect(x - 2, y - 2, 4, 4, M5EPD_Canvas::G15);
			}
			else
			{
				int16_t r = 2;

				if (context->flightState.trackPoints[i].vario > 1)
					r = (context->flightState.trackPoints[i].vario > 2) ? 4 : 3;

				m_pRefCanvas->drawCircle(x, y, r, M5EPD_Canvas::G15);
			}
		}

		i = (i + 1) % MAX_TRACK_HISTORY;
	}

	// draw glider
	{
		int16_t cx = m_x + m_w / 2;
		int16_t cy = m_y + m_h / 2;

		m_pRefCanvas->drawLine(cx - 10, cy, cx, cy - 18, M5EPD_Canvas::G15);
		m_pRefCanvas->drawLine(cx, cy - 18, cx + 10, cy, M5EPD_Canvas::G15);
	}
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

