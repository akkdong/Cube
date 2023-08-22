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
    : Widget(pRefCanvas, 0, 0, 0, 0)
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




/////////////////////////////////////////////////////////////////////////////////
// class Annunciator

Annunciator::Annunciator(M5EPD_Canvas* pRefCanvas)
    : Annunciator(pRefCanvas, 0, 0, 0, 0)
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

void Annunciator::onDraw()
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
        sprintf(sz, "%.2fv", mVoltage);
        m_pRefCanvas->setTextSize(32);
        m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
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
    : ValueBox(pRefCanvas, 0, 0, 0, 0)
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

void ValueBox::init(uint16_t userData)
{
    // set value-box type as user-data
    setUserData(userData);

    // set title & description
    const char *title, *desc;
    unsigned int decimalPlaces = 0;

    switch (userData)
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
    switch (userData)
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

void ValueBox::onDraw()
{
    m_pRefCanvas->fillRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G0);
    m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);

    m_pRefCanvas->setTextDatum(TL_DATUM);
    m_pRefCanvas->setTextSize(m_titleFontSize);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
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
    : ThermalAssist(pRefCanvas, 0, 0, 0, 0)
{

}

ThermalAssist::ThermalAssist(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
    , m_compassPtr(nullptr)
{
}

ThermalAssist::~ThermalAssist()
{
    if (m_compassPtr)
        delete m_compassPtr;
}

int ThermalAssist::update(DeviceContext* context, uint32_t updateHints)
{
    if(m_compassPtr)
        m_compassPtr->update(context, updateHints);

    return 0;
}

void ThermalAssist::init(uint16_t userData)
{
    if (!m_compassPtr)
        m_compassPtr = new Compass(m_pRefCanvas, 0, 0, 160, 160);
}

void ThermalAssist::move(int x, int y, int w, int h)
{
    Widget::move(x, y, w, h);

    if (m_compassPtr)
        m_compassPtr->move(m_x + 4, m_y + 4, 160, 160);
}

void ThermalAssist::show(bool show)
{
    Widget::show(show);
}

void ThermalAssist::draw()
{
    Widget::draw();

    // draw compass
    if (m_compassPtr)
        m_compassPtr->draw();
}

#define ZOOM_FACTOR				(1.8)

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

        #if METHOD_1
		m_pRefCanvas->drawLine(cx - 18, cy, cx, cy - 28, M5EPD_Canvas::G15);
		m_pRefCanvas->drawLine(cx, cy - 28, cx + 18, cy, M5EPD_Canvas::G15);
        #else
        m_pRefCanvas->fillTriangle(cx - 18, cy + 16, cx, cy - 16, cx + 18, cy + 16, M5EPD_Canvas::G15);
        #endif
	}
}






/////////////////////////////////////////////////////////////////////////////////
// class Compass

Compass::Compass(M5EPD_Canvas* pRefCanvas)
    : Compass(pRefCanvas, 0, 0, 0, 0)
{

}

Compass::Compass(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
    , m_heading(0)
    , m_bearing(-1)
    , m_method(1)
{
}

int Compass::update(DeviceContext* context, uint32_t updateHints)
{
    m_heading = context->varioState.heading;
//  m_bearing = context->flightState.bearingNextPoint;
    m_bearing = context->flightState.bearingTakeoff;

    return 0;
}

void Compass::onDraw()
{
    int base_x = this->m_w / 2 + m_x;
    int base_y = this->m_h / 2 + m_y;
    int angle, up = 0;
    int radius = this->m_w / 2 - 4;

    if (m_method == 2 && m_bearing < 0) // bearing < 0 : n/a
        m_method = 1;

    if (m_method == 1)
        up = m_heading;
    else if (m_method == 2) 
        up = m_bearing;
    // else up = 0

    // draw north-letter
    angle = 0 - up; // real-north = rotate counterclockwise

    int point_x = base_x + (int)((radius - 10) * sin(angle * DEG_TO_RAD));
    int point_y = base_y - (int)((radius - 10) * cos(angle * DEG_TO_RAD));

    //int offset_x = letter_n_16.header.w / 2;
    //int offset_y = letter_n_16.header.h / 2;

    m_pRefCanvas->fillRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G0);
    m_pRefCanvas->drawCircle(m_x + m_w / 2, m_y + m_h / 2, radius - 10, M5EPD_Canvas::G15);

    m_pRefCanvas->setTextDatum(CC_DATUM);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
    m_pRefCanvas->setTextSize(32);
    m_pRefCanvas->drawString("N", point_x, point_y);

    // draw heading
    angle = m_heading - up; // rotate counterclockwise
    drawArrow(base_x, base_y, radius - 40, angle);

    // draw bearing
    if (m_bearing >= 0)
    {
        angle = m_bearing - up; // rotate counterclockwise
        drawArrow2(base_x, base_y, radius - 20, angle);
    }
}

void Compass::drawArrow(int cx, int cy, int radius, int angle)
{
    Point points[4];

    points[0].x = cx + (int)(radius * sin(angle * DEG_TO_RAD));
    points[0].y = cy - (int)(radius * cos(angle * DEG_TO_RAD));
    points[1].x = cx + (int)(radius / 3 * sin((angle + 180) * DEG_TO_RAD));
    points[1].y = cy - (int)(radius / 3 * cos((angle + 180) * DEG_TO_RAD));
    points[2].x = cx + (int)(radius * sin((angle - 140) * DEG_TO_RAD));
    points[2].y = cy - (int)(radius * cos((angle - 140) * DEG_TO_RAD));
    points[3].x = cx + (int)(radius * sin((angle + 140) * DEG_TO_RAD));
    points[3].y = cy - (int)(radius * cos((angle + 140) * DEG_TO_RAD));

    #if METHOD_1
    m_pRefCanvas->drawTriangle(
        points[0].x, points[0].y,
        points[1].x, points[1].y,
        points[2].x, points[2].y,
        M5EPD_Canvas::G15);

    m_pRefCanvas->drawTriangle(
        points[0].x, points[0].y,
        points[1].x, points[1].y,
        points[2].x, points[2].y,
        M5EPD_Canvas::G15);
    #else
    m_pRefCanvas->drawLine(points[0].x, points[0].y, points[2].x, points[2].y, M5EPD_Canvas::G15);
    m_pRefCanvas->drawLine(points[2].x, points[2].y, points[1].x, points[1].y, M5EPD_Canvas::G15);
    m_pRefCanvas->drawLine(points[1].x, points[1].y, points[3].x, points[3].y, M5EPD_Canvas::G15);
    m_pRefCanvas->drawLine(points[3].x, points[3].y, points[0].x, points[0].y, M5EPD_Canvas::G15);
    #endif
}

void Compass::drawArrow2(int cx, int cy, int radius, int angle)
{
    Point points[4];

    points[0].x = cx + (int)(radius * sin(angle * DEG_TO_RAD));
    points[0].y = cy - (int)(radius * cos(angle * DEG_TO_RAD));
    points[1].x = cx + (int)((radius - 16) * sin((angle + 12) * DEG_TO_RAD));
    points[1].y = cy - (int)((radius - 16) * cos((angle + 12) * DEG_TO_RAD));
    points[2].x = cx + (int)((radius - 16) * sin((angle - 12) * DEG_TO_RAD));
    points[2].y = cy - (int)((radius - 16) * cos((angle - 12) * DEG_TO_RAD));
    points[3].x = cx + (int)((radius - 6) * sin((angle + 180) * DEG_TO_RAD));
    points[3].y = cy - (int)((radius - 6) * cos((angle + 180) * DEG_TO_RAD));

    #if METHOD_1
    m_pRefCanvas->drawLine(points[0].x, points[0].y, points[1].x, points[1].y, M5EPD_Canvas::G15);
    m_pRefCanvas->drawLine(points[0].x, points[0].y, points[2].x, points[2].y, M5EPD_Canvas::G15);
    m_pRefCanvas->drawLine(points[0].x, points[0].y, points[3].x, points[3].y, M5EPD_Canvas::G15);

    m_pRefCanvas->drawCircle(points[3].x, points[3].y, 6, M5EPD_Canvas::G15);
    #else
    //m_pRefCanvas->drawLine(points[0].x, points[0].y, points[1].x, points[1].y, M5EPD_Canvas::G15);
    //m_pRefCanvas->drawLine(points[1].x, points[1].y, points[2].x, points[2].y, M5EPD_Canvas::G15);
    //m_pRefCanvas->drawLine(points[2].x, points[2].y, points[0].x, points[0].y, M5EPD_Canvas::G15);
    m_pRefCanvas->fillTriangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, M5EPD_Canvas::G15);
    #endif
}


/////////////////////////////////////////////////////////////////////////////////
// class VarioMeter

VarioMeter::VarioMeter(M5EPD_Canvas* pRefCanvas)
    : VarioMeter(pRefCanvas, 0, 0, 0, 0)
{

}

VarioMeter::VarioMeter(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h)
    : Widget(pRefCanvas, x, y, w, h)
    , m_vario(0)
{
}

int VarioMeter::update(DeviceContext* context, uint32_t updateHints)
{
    m_vario = context->varioState.speedVertActive;

    return 0;
}

void VarioMeter::onDraw()
{
	#define BAR_H	(20)
	#define BAR_G	(6)
	int16_t h = (m_h - 8) / 2;
	int16_t w = m_w - 40;
	int16_t x = m_x + 20;
	int16_t y = m_y + m_h / 2;
	int16_t sign = 1;

	float vario = m_vario;
	if (vario > 0) 
	{
		sign = 1;
		vario = (vario > 5.0) ? 5.0 : vario;
	}
	else
	{
		sign = -1;
		vario = (vario < -5.0) ? 5.0 : -vario;
	}

	// vario --> height
	// vario : 5.0 = y : h, y = vario / 5.0 * h;
	int16_t v_offset = (int16_t)(vario / 5.0 * h);

    //m_pRefCanvas->fillRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G0);
    //m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);
    m_pRefCanvas->drawFastHLine(x, y, w, M5EPD_Canvas::G15);

	for (int16_t yy = 0; yy <= h; yy += (BAR_H + BAR_G))
	{
		if (v_offset <= yy)
			break;

		if (sign > 0)
		{
			m_pRefCanvas->fillRect(x, y - (yy + BAR_H), w, BAR_H, M5EPD_Canvas::G15);

		}
		else
		{
			m_pRefCanvas->drawRect(x, y + yy, w, BAR_H, M5EPD_Canvas::G15);
		}
		
		w -= 16;
        x += 8;
	}
}






/////////////////////////////////////////////////////////////////////////////////
// class MessageBox

MessageBox::MessageBox(M5EPD_Canvas* pRefCanvas)
    : MessageBox(pRefCanvas, 0, 0, 0, 0)
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

void MessageBox::onDraw()
{
    m_pRefCanvas->fillRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G15);
    //m_pRefCanvas->drawRect(m_x, m_y, m_w, m_h, M5EPD_Canvas::G0);

    m_pRefCanvas->setTextDatum(CC_DATUM);
    m_pRefCanvas->setTextSize(48);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G0);
    m_pRefCanvas->drawString(m_msg, m_x + m_w / 2, m_y + m_h / 2);
}
