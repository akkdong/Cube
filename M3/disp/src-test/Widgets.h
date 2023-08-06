// Widgets.h
//

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#define WSTATE_VISIBLE      (0x8000)
#define WSTATE_UPDATED      (0x4000)

#define WSTATE_USER_MASK    (0x00FF)


/////////////////////////////////////////////////////////////////////////////////
//

class M5EPD_Canvas;
class DeviceContext;

/////////////////////////////////////////////////////////////////////////////////
// class Widget

class Widget
{
public:
    // constructor & destructor
    //Widget();
    Widget(M5EPD_Canvas* pRefCanvas);
    Widget(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    //
    void setDirty(bool dirty);
    void setPosition(int x, int y);
    void move(int x, int y, int w, int h);
    void show(bool show);

    bool isDirty() { return m_flag & WSTATE_UPDATED ? true : false; }
    bool isVisible() { return m_flag & WSTATE_VISIBLE ? true : false; }

    void setUserData(uint16_t data) { m_flag = (m_flag & (~WSTATE_USER_MASK)) | data; }
    uint16_t getUserData() { return (uint16_t)(m_flag & WSTATE_USER_MASK); }

    //
    virtual int update(DeviceContext* context);
    virtual void draw() {}

    //
    virtual void onDraw();
    //
    virtual void onTouchDown(int x, int y);
    virtual void onTouchMove(int x, int y);
    virtual void onTouchUp(int x, int y);
    //
    virtual void onKeyPress(unsigned short key);
    virtual void onKeyLongPress(unsigned short key);
    virtual void onKeyRelease(unsigned short key);

protected:

protected:
    M5EPD_Canvas* m_pRefCanvas;
    int m_x, m_y, m_w, m_h;
    uint32_t m_flag; // visible, dirty, ...
};


/////////////////////////////////////////////////////////////////////////////////
// class Annunciator

class Annunciator : public Widget
{
public:
    Annunciator(M5EPD_Canvas* pRefCanvas);
    Annunciator(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    //
    virtual int update(DeviceContext* context);
    //
    virtual void onDraw();


protected:
    const char * getTimeString(char* str, time_t t, bool includeSecond);
};


/////////////////////////////////////////////////////////////////////////////////
// class ValueBox

class ValueBox : public Widget
{
public:
    enum VType {
        UNDEF = 0,
        ALTITUDE_GROUND,
        ALTITUDE_BARO,
        ALTITUDE_AGL,       // Height Above Ground Level
        ALTITUDE_PROFILE,
        SPEED_GROUND,
        SPEED_AIR,
        SPEED_VERTICAL,
        SPEED_VERTICAL_LAZY,
        TRACK_HEADING,
        TARCK_BEARING,
        TIME_FLIGHT,
        TIME_CURRENT,
        TIME_TO_NEXT_WAYPOINT,
        TIME_REMAIN,
        DISTANCE_TAKEOFF,
        DISTANCE_LANDING,
        DISTANCE_NEXT_WAYPOINT,
        DISTANCE_FLIGHT,    // odometer
        GLIDE_RATIO,
        COMPASS,
        VSPEED_BAR,
        VSPEED_PROFILE,
        TRACK_FLIGHT,
        SENSOR_PRESSURE,
        SENSOR_TEMPERATURE,
        SENSOR_HUMIDITY,
        END_OF_BOX,
        BOX_COUNT = END_OF_BOX,        
    };

    enum BType {
        BORDER_NONE = 0,
        BORDER_LEFT = (1 << 0),
        BORDER_RIGHT = (1 << 1),
        BORDER_TOP = (1 << 2),
        BORDER_BOTTOM = (1 << 3),
        BORDER_ALL = BORDER_LEFT | BORDER_RIGHT | BORDER_TOP | BORDER_BOTTOM,
    };
        
public:
    //
    ValueBox(M5EPD_Canvas* pRefCanvas);
    ValueBox(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    //
    void setTitle(const char* title) { m_title = title; }
    void setDescription(const char* desc) { m_desc = desc; }
    void setValue(double value, unsigned int decimalPlace) { m_value = value; m_decimalPlace = decimalPlace; }

    //
    static void setFontSize(int titleSize, int valueSize);

    //
    virtual int update(DeviceContext* context);
    //
    virtual void onDraw();

protected:
    const char *m_title;
    const char *m_desc;
    double m_value;
    unsigned int m_decimalPlace;

    static int m_titleFontSize;
    static int m_valueFontSize;
};



/////////////////////////////////////////////////////////////////////////////////
// class ThermalAssist

class ThermalAssist : public Widget
{
public:
    //
    ThermalAssist(M5EPD_Canvas* pRefCanvas);
    ThermalAssist(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    //
    virtual int update(DeviceContext* context);
    //
    virtual void onDraw();
};



/////////////////////////////////////////////////////////////////////////////////
// class Compass

class Compass : public Widget
{
public:
    //
    Compass(M5EPD_Canvas* pRefCanvas);
    Compass(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    //
    virtual int update(DeviceContext* context);
    //
    virtual void onDraw();
};





/////////////////////////////////////////////////////////////////////////////////
// class VarioMeter

class VarioMeter : public Widget
{
public:
    //
    VarioMeter(M5EPD_Canvas* pRefCanvas);
    VarioMeter(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    //
    virtual int update(DeviceContext* context);
    //
    virtual void onDraw();
};


#endif // __WIDGETS_H__
