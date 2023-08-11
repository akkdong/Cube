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
    , m_flag(0)
{
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

    //m_flag = m_flag | WSTATE_UPDATED;
}

void Widget::move(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;

    //m_flag = m_flag | WSTATE_UPDATED;
}

void Widget::show(bool show)
{
    if (show)
        m_flag = m_flag | WSTATE_VISIBLE;
    else
        m_flag = m_flag & (~WSTATE_VISIBLE);
}

int Widget::update(DeviceContext *context, uint32_t updateHints)
{
    return 0;
}

void Widget::draw()
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

int Annunciator::update(DeviceContext* context, uint32_t updateHints)
{
    int isUpdated = 0;

    // 109876543210           
    // BBGGSSVVCCCC
    // BT
    //   GPS
    //     SD
    //       VOLUME
    //         BATTERY
    uint32_t state = 0;
    state |= (uint32_t)context->deviceState.statusBT << 10;
    state |= (uint32_t)context->deviceState.statusGPS << 8;
    state |= (uint32_t)context->deviceState.statusSDCard << 6;
    state |= (uint32_t)(context->volume.vario > 0 ? 1 : 0) << 4;
    
    uint32_t bat = 4; // empty, 1/3, 1/2, full, charging
    if (context->deviceState.batteryPower < 4.28)
    {
        bat = 3;
        if (context->deviceState.batteryPower < 3.6)
        {
            bat = 2;
            if (context->deviceState.batteryPower < 3.2)
            {
                bat = 1;
                if (context->deviceState.batteryPower < 2.8)
                {
                    bat = 0;
                }
            }
        }
    }

    state |= bat;
    if (state != mState)
    {
        mState = state;
        isUpdated = 1;
    }

    if ((int)(mVoltage * 100) != (int)(context->deviceState.batteryPower * 100))
    {
        mVoltage = context->deviceState.batteryPower;
        isUpdated = 1;
    }

    time_t now = time(NULL);
    if (now != mLastTime)
    {
        mLastTime = now;
        isUpdated = 1;
    }

    return isUpdated;
}

void Annunciator::draw()
{
    #define ICON_W      48
    #define ICON_H      48
    #define STATUS_H    60
    #define ICON_MARGIN  8
    #define ICON_SPACE   0

    //m_pRefCanvas->drawRect(m_x, m_y, M5EPD_PANEL_W, STATUS_H, M5EPD_Canvas::G15);
    m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);
    int x = m_x + 8;
    int y = m_y + (m_h - ICON_H) / 2;
    // WOLF
    {
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, (const uint8_t *)ImageResource_logo_wolf_small_48x48);
        x += ICON_W /*+ ICON_MARGIN*/;
    }
    // BT
    uint8_t state = (mState >> 10) & 0x03;
    if( state > 0)
    {
        const uint8_t *img = state > 1 ? ImageResource_bluetooth_paried_48x48 : ImageResource_bluetooth_unpaired_48x48;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // GPS
    state = (mState >> 8) & 0x03;
    {
        const uint8_t *img = state > 0 ? ImageResource_gps_fixed_48x48 : ImageResource_gps_unfixed_48x48;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // SD
    state = (mState >> 6) & 0x03;
    if (state > 0)
    {
        const uint8_t *img = state > 1 ? ImageResource_sd_logging_48x48 : ImageResource_sd_exist_48x48;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // VOLUME
    state = (mState >> 4) & 0x03;
    {
        const uint8_t *img = state > 0 ? ImageResource_volume_on_48x48 : ImageResource_volume_mute_48x48;
        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
        x += ICON_W + ICON_SPACE;
    }
    // BATTERY
    state = mState & 0x0F;
    {
        const uint8_t *img;
        switch (state)
        {
        case 4: img = ImageResource_battery_charging_48x48; break;
        case 3: img = ImageResource_battery_full_48x48; break;
        case 2: img = ImageResource_battery_half_48x48; break;
        case 1: img = ImageResource_battery_low_48x48; break;
        case 0:
        default:  img = ImageResource_battery_empty_48x48; break;
        }

        m_pRefCanvas->pushImage(x, y, ICON_W, ICON_H, img);
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
        getTimeString(sz, mLastTime, true);

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
//

class NumberData : public ValueProvider
{
public:
    NumberData(int decimalPlace) : m_value(0), m_decimalPlace(decimalPlace) {}

public:
    void setValue(float value) override {
        m_value = value;
    }
    void setValue(int16_t value) override {
        m_value = value;
    }

    const char * getValue(char * buf, int bufLen) override {
        sprintf(buf, "%.*f", m_decimalPlace, m_value);
        return buf;
    }

protected:
    float m_value;
    int m_decimalPlace;
};

class TimeStringData : public ValueProvider
{
public:
    TimeStringData(bool shortStr) : m_value(0), m_shortStr(shortStr) {}

public:
    void setValue(time_t value) override {
        m_value = value;
    }

    const char * getValue(char * buf, int bufLen) override {
        struct tm * _tm = localtime(&m_value);    
        if (m_shortStr)
            sprintf(buf, "%02d:%02d:%02d", _tm->tm_hour, _tm->tm_min,_tm->tm_sec);
        else
            sprintf(buf, "%02d:%02d", _tm->tm_hour * 3600 + _tm->tm_min, _tm->tm_sec);

        return buf;
    }

protected:
    time_t m_value;
    bool m_shortStr;
};



/////////////////////////////////////////////////////////////////////////////////
// class ValueBox

int ValueBox::m_titleFontSize = 32;
int ValueBox::m_valueFontSize = 70;

ValueBox::ValueBox(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas, 0, 0, 0, 0)
    , m_title("")
    , m_desc("")
    , m_pValueProvider(nullptr)
{

}

ValueBox::ValueBox(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
    , m_title("")
    , m_desc("")
    , m_pValueProvider(nullptr)
{

}

ValueBox::~ValueBox()
{
    if (m_pValueProvider)
        delete m_pValueProvider;
}

void ValueBox::init(uint16_t vType)
{
    // set value-box type as user-data
    setUserData(vType);

    // set title & description
    const char *title, *desc;
    unsigned int decimalPlaces = 0;

    switch (vType)
    {
    case ALTITUDE_GROUND:
        title = "Alt Ground";
        desc = "m";
        break;
    case ALTITUDE_BARO:
        title = "Alt Baro";
        desc = "m";
        break;
    case ALTITUDE_AGL:
        title = "AGL";
        desc = "m";
        break;
    case SPEED_GROUND:
        title = "Speed Ground";
        desc = "km/s";
        break;
    case SPEED_AIR:
        title = "Speed AIR";
        desc = "km/s";
        break;
    case SPEED_VERTICAL:
        title = "Variometer";
        desc = "m/s";
        decimalPlaces = 2;
        break;
    case SPEED_VERTICAL_LAZY:
        title = "Alt Ground";
        desc = "m/s";
        decimalPlaces = 2;
        break;
    case TRACK_HEADING:
        title = "Heading";
        desc = "Deg";
        break;
    case TARCK_BEARING:
        title = "Bearing";
        desc = "Deg";
        break;
    case TIME_FLIGHT:
        title = "Time Flight";
        desc = "hh:mm:ss";
        break;
    case TIME_CURRENT:
        title = "Time";
        desc = "hh:mm:ss";
        break;
    case TIME_TO_NEXT_WAYPOINT:
        title = "Time Next";
        desc = "mm:ss";
        break;
    case TIME_REMAIN:
        title = "Time Remain";
        desc = "mm:ss";
        break;
    case DISTANCE_TAKEOFF:
        title = "Dist. Takeoff";
        desc = "km";
        decimalPlaces = 1;
        break;
    case DISTANCE_LANDING:
        title = "Dist. Landing";
        desc = "km";
        decimalPlaces = 1;
        break;
    case DISTANCE_NEXT_WAYPOINT:
        title = "Dist. Next";
        desc = "km";
        decimalPlaces = 1;
        break;
    case DISTANCE_FLIGHT:
        title = "Dist. Flight";
        desc = "km";
        decimalPlaces = 1;
        break;
    case GLIDE_RATIO:
        title = "L/D";
        desc = "";
        decimalPlaces = 1;
        break;
    case SENSOR_PRESSURE:
        title = "Pressure";
        desc = "Pa";
        break;
    case SENSOR_TEMPERATURE:
        title = "Temp";
        desc = "Celsius";
        decimalPlaces = 1;
        break;
    case SENSOR_HUMIDITY:
        title = "Humidity";
        desc = "%";
        break;
    default:
        return;
    }

    setTitle(title);
    setDescription(desc);

    // set value-provider
    switch (vType)
    {
    case TIME_FLIGHT:
    case TIME_CURRENT:
        m_pValueProvider = new TimeStringData(false);
        break;
    case TIME_TO_NEXT_WAYPOINT:
    case TIME_REMAIN:
        m_pValueProvider = new TimeStringData(true);
        break;
    default:
        m_pValueProvider = new NumberData(decimalPlaces);
        break;
    }
}

void ValueBox::setFontSize(int titleSize, int valueSize)
{
    m_titleFontSize = titleSize;
    m_valueFontSize = valueSize;
}

int ValueBox::update(DeviceContext* context, uint32_t updateHints)
{
    if (!m_pValueProvider)
        return 0;

    // update value and title, desc if necessary
    switch (getUserData())
    {
    case ValueBox::VType::ALTITUDE_GROUND:
        m_pValueProvider->setValue(context->varioState.altitudeGPS);
        break;
    case ValueBox::VType::ALTITUDE_BARO:
        m_pValueProvider->setValue(context->varioState.altitudeBaro);
        break;
    case ValueBox::VType::ALTITUDE_AGL:
        m_pValueProvider->setValue(context->varioState.altitudeAGL);
        break;
    case ValueBox::VType::SPEED_GROUND:
        m_pValueProvider->setValue(context->varioState.speedGround);
        break;
    case ValueBox::VType::SPEED_AIR:
        return 0;
    case ValueBox::VType::SPEED_VERTICAL:
        m_pValueProvider->setValue(context->varioState.speedVertActive);
        break;
    case ValueBox::VType::SPEED_VERTICAL_LAZY:
        m_pValueProvider->setValue(context->varioState.speedVertLazy);
        break;
    case ValueBox::VType::TRACK_HEADING:
        m_pValueProvider->setValue(context->varioState.heading);
        break;
    case ValueBox::VType::TARCK_BEARING:
        return 0;
    case ValueBox::VType::TIME_FLIGHT:
        m_pValueProvider->setValue((time_t)context->flightState.flightTime);
        return 0;
    case ValueBox::VType::TIME_CURRENT:
        m_pValueProvider->setValue(context->varioState.timeCurrent);
        break;
    case ValueBox::VType::TIME_TO_NEXT_WAYPOINT:
        return 0;
    case ValueBox::VType::TIME_REMAIN:
    case ValueBox::VType::DISTANCE_TAKEOFF:
    case ValueBox::VType::DISTANCE_LANDING:
    case ValueBox::VType::DISTANCE_NEXT_WAYPOINT:
    case ValueBox::VType::DISTANCE_FLIGHT:
        return 0;
    case ValueBox::VType::GLIDE_RATIO:
        m_pValueProvider->setValue(context->flightState.glideRatio);
        break;
    case ValueBox::VType::SENSOR_PRESSURE:
        m_pValueProvider->setValue(context->varioState.pressure);
        break;
    case ValueBox::VType::SENSOR_TEMPERATURE:
        m_pValueProvider->setValue(context->deviceState.temperature);
        break;
    case ValueBox::VType::SENSOR_HUMIDITY:
        m_pValueProvider->setValue(context->deviceState.humidity);
        break;
    default:
        return 0;
    }

    return 1;
}

void ValueBox::draw()
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
    m_pValueProvider->getValue(sz, sizeof(sz));
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

int ThermalAssist::update(DeviceContext* context, uint32_t updateHints)
{
    return 0;
}


#define ZOOM_FACTOR				(1.0)

void ThermalAssist::draw()
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
				m_pRefCanvas->drawRect(x - 3, y - 3, 6, 6, M5EPD_Canvas::G15);
			}
			else
			{
				int16_t r = 4;

				if (context->flightState.trackPoints[i].vario > 1)
					r = (context->flightState.trackPoints[i].vario > 2) ? 8 : 6;

				m_pRefCanvas->drawCircle(x, y, r, M5EPD_Canvas::G15);
			}
		}

		i = (i + 1) % MAX_TRACK_HISTORY;
	}

	// draw glider
	{
		int16_t cx = m_x + m_w / 2;
		int16_t cy = m_y + m_h / 2;

		m_pRefCanvas->drawLine(cx - 20, cy, cx, cy - 28, M5EPD_Canvas::G15);
		m_pRefCanvas->drawLine(cx, cy - 28, cx + 20, cy, M5EPD_Canvas::G15);
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

int Compass::update(DeviceContext* context, uint32_t updateHints)
{
    return 0;
}

void Compass::draw()
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

int VarioMeter::update(DeviceContext* context, uint32_t updateHints)
{
    return 0;
}

void VarioMeter::draw()
{

}






/////////////////////////////////////////////////////////////////////////////////
// class MessageBox

MessageBox::MessageBox(M5EPD_Canvas* pRefCanvas)
    : Widget(pRefCanvas)
{

}

MessageBox::MessageBox(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
{
}

void MessageBox::setMessage(const char* msg)
{
    m_msg = msg;
}

int MessageBox::update(DeviceContext* context, uint32_t updateHints)
{
    return 0;
}

void MessageBox::draw()
{
    m_pRefCanvas->fillRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G0);
    m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);

    m_pRefCanvas->setTextDatum(CC_DATUM);
    m_pRefCanvas->setTextSize(48);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
    m_pRefCanvas->drawString(m_msg, m_x + m_w / 2, m_y + m_h / 2);
}
