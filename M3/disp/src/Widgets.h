// Widgets.h
//

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#define WSTATE_WIDGET_MASK      (0xFFFF0000)
#define WSTATE_VISIBLE          (0x00008000)
#define WSTATE_UPDATED          (0x00004000)
#define WSTATE_USER_MASK        (0x000000FF)


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
    void setUserData(uint32_t data) { m_flag = (m_flag & (~WSTATE_USER_MASK)) | (data & WSTATE_USER_MASK); }

    uint32_t getUserData() { return (uint32_t)(m_flag & WSTATE_USER_MASK); }

    bool isDirty() { return m_flag & WSTATE_UPDATED ? true : false; }
    bool isVisible() { return m_flag & WSTATE_VISIBLE ? true : false; }

    //
    virtual void init(uint32_t flag) {}
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

    // flag: XXXX-XXXX
    //  FLAG_VISIBLE                8000-0000
    //  FLAG_UPDATE                 4000-0000
    //  FLAG_WIDGET                 0FFF-0000
    //  FLAG_USER                   0000-FFFF
    uint32_t m_flag;
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


#define LABEL_STATE_MASK        (0x0F000000)
#define UNIT_STATE_MASK         (0x00F00000)
#define VALUE_STATE_MASK        (0x000F0000)
#define LABEL_STATE_OFFSET      (24)
#define UNIT_STATE_OFFSET       (20)
#define VALUE_STATE_OFFSET      (16)


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
        // WIDGET_FLAG_MASK = 0x0FFF0000
        BORDER_MASK         = 0xF0000000,
        BORDER_LEFT         = 0x10000000,
        BORDER_TOP          = 0x20000000,
        BORDER_RIGHT        = 0x40000000,
        BORDER_BOTTOM       = 0x80000000,
        BORDER_ALL          = 0xF0000000,

        //
        // alignment position
        //
        // +--------------------------------------------+
        // |X1                   X2                   X3|
        // |                                            |
        // |                                            |
        // |X4                   X5                   X6|
        // |                                            |
        // |                                            |
        // |X7                   X8                   X9|
        // +--------------------------------------------+
        //
        // X0 : EMPTY               : 0
        // X1 : TOP | LEFT          : 1
        // X2 : TOP | CENTER        : 2
        // X3 : TOP | RIGHT         : 3
        // X4 : MIDDLE | LEFT       : 4
        // X5 : MIDDLE | CENTER     : 5
        // X6 : MIDDLE | RIGHT      : 6
        // X7 : BOTTOM | LEFT       : 7
        // X8 : BOTTOM | CENTER     : 8
        // X9 : BOTTOM | RIGHT      : 9
        //

        //LABEL_STATE_MASK    = 0x0F000000,
        //UNIT_STATE_MASK     = 0x00F00000,
        //VALUE_STATE_MASK    = 0x000F0000,
        //LABEL_STATE_OFFSET  = 48,
        //UNIT_STATE_OFFSET   = 40,
        //VALUE_STATE_OFFSET  = 32,

        ALIGN_LABEL_TL      = 0x01000000, // TOP_LEFT, TL_DAUM(0)
        ALIGN_LABEL_TC      = 0x02000000, // TOP_CENTER, TC_DATUM(1)
        ALIGN_LABEL_TR      = 0x03000000, // TOP_RIGHT, TR_DATUM(2)
        ALIGN_LABEL_ML      = 0x04000000, // MIDDLE_LEFT, ML_DATUM(3)
        ALIGN_LABEL_MC      = 0x05000000, // MIDDLE_CENTER, MC_DATUM(4)
        ALIGN_LABEL_MR      = 0x06000000, // MIDDLE_RIGHT, MR_DATUM(5)
        ALIGN_LABEL_BL      = 0x07000000, // BOTTOM_LEFT, BL_DATUM(6)
        ALIGN_LABEL_BC      = 0x08000000, // BOTTOM_CENTER, BC_DATUM(7)
        ALIGN_LABEL_BR      = 0x09000000, // BOTTOM_RIGHT, BR_DATUM(8)

        ALIGN_UNIT_TL       = 0x00100000, // TOP_LEFT, TL_DAUM(0)
        ALIGN_UNIT_TC       = 0x00200000, // TOP_CENTER, TC_DATUM(1)
        ALIGN_UNIT_TR       = 0x00300000, // TOP_RIGHT, TR_DATUM(2)
        ALIGN_UNIT_ML       = 0x00400000, // MIDDLE_LEFT, ML_DATUM(3)
        ALIGN_UNIT_MC       = 0x00500000, // MIDDLE_CENTER, MC_DATUM(4)
        ALIGN_UNIT_MR       = 0x00600000, // MIDDLE_RIGHT, MR_DATUM(5)
        ALIGN_UNIT_BL       = 0x00700000, // BOTTOM_LEFT, BL_DATUM(6)
        ALIGN_UNIT_BC       = 0x00800000, // BOTTOM_CENTER, BC_DATUM(7)
        ALIGN_UNIT_BR       = 0x00900000, // BOTTOM_RIGHT, BR_DATUM(8)

        ALIGN_VALUE_TL      = 0x00010000, // TOP_LEFT, TL_DAUM(0)
        ALIGN_VALUE_TC      = 0x00020000, // TOP_CENTER, TC_DATUM(1)
        ALIGN_VALUE_TR      = 0x00030000, // TOP_RIGHT, TR_DATUM(2)
        ALIGN_VALUE_ML      = 0x00040000, // MIDDLE_LEFT, ML_DATUM(3)
        ALIGN_VALUE_MC      = 0x00050000, // MIDDLE_CENTER, MC_DATUM(4)
        ALIGN_VALUE_MR      = 0x00060000, // MIDDLE_RIGHT, MR_DATUM(5)
        ALIGN_VALUE_BL      = 0x00070000, // BOTTOM_LEFT, BL_DATUM(6)
        ALIGN_VALUE_BC      = 0x00080000, // BOTTOM_CENTER, BC_DATUM(7)
        ALIGN_VALUE_BR      = 0x00090000, // BOTTOM_RIGHT, BR_DATUM(8)
    };

public:
    //
    ValueBox(M5EPD_Canvas* pRefCanvas);
    ValueBox(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);
    virtual ~ValueBox();

    //
    void setTitle(const char* title) { m_title = title; }
    void setDescription(const char* desc) { m_desc = desc; }
    void setBoxType(uint32_t data) { m_flag = (m_flag & (~WSTATE_WIDGET_MASK)) | (data & WSTATE_WIDGET_MASK); }

    //
    static void setFontSize(int titleSize, int valueSize);

    //
    virtual void init(uint32_t flag);
    virtual int update(DeviceContext* context, uint32_t updateHints);


protected:
    //
    void calcDrawPosition(uint8_t datum, int &x, int &y);

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
    virtual void init(uint32_t flag);
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
