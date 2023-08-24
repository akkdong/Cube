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
class ValueProvider;

class Application;

class Annunciator;
class ValueBox;
class ThermalAssist;
class Compass;
class VarioMeter;
class MessageBox;


/////////////////////////////////////////////////////////////////////////////////
// class Widget

class Widget
{
    friend class Application;
public:
    // constructor & destructor
    //Widget();
    Widget(M5EPD_Canvas* pRefCanvas);
    Widget(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);
    virtual ~Widget() {}

    //
    void setDirty(bool dirty);
    void setPosition(int x, int y);
    void setUserData(uint16_t data) { m_flag = (m_flag & (~WSTATE_USER_MASK)) | data; }

    uint16_t getUserData() { return (uint16_t)(m_flag & WSTATE_USER_MASK); }

    bool isDirty() { return m_flag & WSTATE_UPDATED ? true : false; }
    bool isVisible() { return m_flag & WSTATE_VISIBLE ? true : false; }

    //
    virtual void init(uint16_t userData) {}
    virtual void move(int x, int y, int w, int h);
    virtual void show(bool show);
    virtual void draw() { onDraw(); }
    virtual int update(DeviceContext* context, uint32_t updateHints) { return 0; }

protected:
    //
    virtual void onDraw() {}

    //
    virtual void onTouchDown(int x, int y) {}
    virtual void onTouchMove(int x, int y) {}
    virtual void onTouchUp(int x, int y) {}
    //
    virtual void onKeyPressed(uint32_t key) {}
    virtual void onKeyLongPressed(uint32_t key) {}
    virtual void onKeyReleased(uint32_t key) {}

protected:
    M5EPD_Canvas *m_pRefCanvas;
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
    virtual int update(DeviceContext* context, uint32_t updateHints);

protected:
    //
    virtual void onDraw();

protected:
    const char * getTimeString(char* str, time_t t, bool includeSecond);

    // BBGGSSVVCCCC
    uint32_t mState;
    float mVoltage;
    time_t mLastTime;
};



/////////////////////////////////////////////////////////////////////////////////
// class ValueProvider

class ValueProvider
{
public:
    virtual void setValue(float value) {};
    virtual void setValue(int16_t value) {};
    virtual void setValue(time_t value) {};
    virtual void setValue(float value1, float value2) {};

    virtual const char * getValue(char * buf, int bufLen) = 0;
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
        SPEED_GROUND,
//      SPEED_AIR,
        SPEED_VERTICAL,
//      SPEED_VERTICAL_LAZY,
        TRACK_HEADING,
        TARCK_BEARING_TAKEOFF,
        TARCK_BEARING_NEXT,
//      TARCK_BEARING_LANDING,
        TIME_FLIGHT,
        TIME_CURRENT,
        TIME_TO_NEXT_WAYPOINT,
        TIME_REMAIN,
        DISTANCE_TAKEOFF,
        DISTANCE_LANDING,
        DISTANCE_NEXT_WAYPOINT,
        DISTANCE_FLIGHT,    // odometer
        GLIDE_RATIO,
        SENSOR_PRESSURE,
        SENSOR_TEMPERATURE,
        SENSOR_HUMIDITY,
        SENSOR_HT,
        VTYPE_COUNT,
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
    virtual ~ValueBox();

    //
    void setTitle(const char* title) { m_title = title; }
    void setDescription(const char* desc) { m_desc = desc; }

    //
    static void setFontSize(int titleSize, int valueSize);

    //
    virtual void init(uint16_t userData);
    virtual int update(DeviceContext* context, uint32_t updateHints);


protected:
    //
    virtual void onDraw();

protected:
    const char *m_title;
    const char *m_desc;
    ValueProvider *m_pValueProvider;

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
    ~ThermalAssist();

    //
    virtual void init(uint16_t userData);
    virtual void move(int x, int y, int w, int h);
    virtual void show(bool show);
    virtual void draw();
    
    virtual int update(DeviceContext* context, uint32_t updateHints);

protected:
    //
    virtual void onDraw();

protected:
    Compass * m_compassPtr;
};



/////////////////////////////////////////////////////////////////////////////////
// class Compass

class Compass : public Widget
{
public:
    //
    Compass(M5EPD_Canvas* pRefCanvas);
    Compass(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    struct Point {
        int x, y;
    };

    //
    virtual int update(DeviceContext* context, uint32_t updateHints);

protected:
    //
    void drawArrow(int cx, int cy, int radius, int angle);
    void drawArrow2(int cx, int cy, int radius, int angle);

    //
    virtual void onDraw();

protected:
    int m_heading, m_bearing;
    int m_method; // up is north(method:0), heading(method:1), bearing(method:2)
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
    virtual int update(DeviceContext* context, uint32_t updateHints);

protected:
    //
    virtual void onDraw();

protected:
    float m_vario;
};





/////////////////////////////////////////////////////////////////////////////////
// class MessageBox

class MessageBox : public Widget
{
public:
    //
    MessageBox(M5EPD_Canvas* pRefCanvas);
    MessageBox(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);

    void setMessage(const char* msg);

    //
    virtual int update(DeviceContext* context, uint32_t updateHints);

protected:
    //
    virtual void onDraw();

protected:
    String m_msg;
};


#endif // __WIDGETS_H__
