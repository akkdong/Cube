// widget.h
//

#ifndef __WIDGET_H__
#define __WIDGET_H__

/*
widget
  annunciator
    logo
    status: gps, bluetooth, sd, sound
    battery-power
    current-time
  text-box
    title, description, value
    
  compass
    property: north, heading, target, target-next
  variometer    
  vario-profile
  altitude-profile
  thermal-assistant
*/

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#include <stdlib.h>
#endif
#include "lvgl.h"
#include "displayBase.h"


////////////////////////////////////////////////////////////////////////////////////
// struct WidgetUpdater

class Annunciator;
class NumberBox;
class ProfileWidget;
class CompassWidget;
class VariometerWidget;
class ThermalAssistant;

struct WidgetUpdater
{
    virtual void onUpdate(Annunciator *) = 0;
    virtual void onUpdate(NumberBox *) = 0;
    virtual void onUpdate(ProfileWidget *) = 0;
    virtual void onUpdate(CompassWidget *) = 0;
    virtual void onUpdate(VariometerWidget *) = 0;
    virtual void onUpdate(ThermalAssistant *) = 0;
};



////////////////////////////////////////////////////////////////////////////////////
// class Widget

class Widget : public DisplayObject
{
public:
    Widget();
    virtual ~Widget();

    enum Type {
        WIDGET_NUMBER_BOX,
        WIDGET_PROFILE,
        WIDGET_COMPASS,
        WIDGET_VARIOMETER,
        WIDGET_THERMAL_ASSISTANT,
    };

public:
    void                    setUpdater(WidgetUpdater* updater) { this->updater = updater; }

    //
    static void             init();

protected:
    lv_obj_t*               createObject(lv_obj_t* parent) override;

protected:
    //
    WidgetUpdater*          updater;

    //
    static lv_style_t*      style_default;
};



////////////////////////////////////////////////////////////////////////////////////
// class Annunciator

class Annunciator : public Widget
{
public:
    Annunciator();

public:
    //
    void                    update() override;

    //
    void                    setStatus(const char* status);
    void                    setClock(const char* text);
    void                    setFont(const lv_font_t* font);

protected:
    lv_obj_t*               createObject(lv_obj_t* parent) override;

protected:
    #if 0
    lv_obj_t*               logo;
    lv_obj_t*               gps;        // invalid, unfixed, fixed
    lv_obj_t*               bluetooth;  // invalid, disconnected, connected
    lv_obj_t*               sdcard;     // invalid, no-card, ready, logging
    lv_obj_t*               volume;     // mute, low, middle, loud
    #endif

    lv_obj_t*               status;      // ? icon, voltage, percentage
    lv_obj_t*               clock;      // [YYYY-MM-DD] HH-MM-DD
};


////////////////////////////////////////////////////////////////////////////////////
// class NumberBox

class NumberBox : public Widget
{
public:
    NumberBox();

    enum BoxType {
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

    enum BorderType {
        BORDER_NONE = 0,
        BORDER_LEFT = (1 << 0),
        BORDER_RIGHT = (1 << 1),
        BORDER_TOP = (1 << 2),
        BORDER_BOTTOM = (1 << 3),
        BORDER_ALL = BORDER_LEFT | BORDER_RIGHT | BORDER_TOP | BORDER_BOTTOM,
    };

public:
    //
    void                    update() override;

    //
    BoxType                 getType() { return boxType; }
    void                    setType(uint32_t type);

    void                    setValue(const char* value);

protected:
    void                    onCreate() override;

    const char*             getTitle(BoxType type);
    const char*             getDescription(BoxType type);

    void                    setTitle(const char* title);
    void                    setDescription(const char* desc);


protected:
    lv_obj_t*               titleObj;
    lv_obj_t*               descObj;
    lv_obj_t*               valueObj;

    BorderType              borderType;
    const char*             titleText;
    const char*             descText;
    BoxType                 boxType;

    static const lv_font_t* font_title;
    static const lv_font_t* font_subtitle;
    static const lv_font_t* font_body;
};


////////////////////////////////////////////////////////////////////////////////////
// class ProfileWidget

class ProfileWidget : public Widget
{
public:
    enum DataType {
        UNDEFINED,
        VSPEED,
        ALTITUDE,
    };

public:
    ProfileWidget();

public:
    //
    void                    update() override;

    //
    void                    setDataType(DataType type) { dataType = type; }

protected:
    lv_obj_t*               createObject(lv_obj_t* parent) override;


protected:
    DataType                dataType;
};



////////////////////////////////////////////////////////////////////////////////////
// class CanvasWidget

class CanvasWidget : public Widget
{
public:
    CanvasWidget(int width, int height);
    ~CanvasWidget();

public:
    //
    void                    update() override;

    //
    void                    drawRect(lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, const lv_draw_rect_dsc_t * draw_dsc);
    void                    drawText(lv_coord_t x, lv_coord_t y, lv_coord_t max_w, lv_draw_label_dsc_t * draw_dsc, const char * txt);
    void                    drawImage(lv_coord_t x, lv_coord_t y, const void * src, const lv_draw_img_dsc_t * draw_dsc);
    void                    drawLine(const lv_point_t points[], uint32_t point_cnt, const lv_draw_line_dsc_t * draw_dsc);
    void                    drawArc(lv_coord_t x, lv_coord_t y, lv_coord_t r, int32_t start_angle, int32_t end_angle, const lv_draw_arc_dsc_t * draw_dsc);
    void                    drawPolygon(const lv_point_t points[], uint32_t point_cnt, const lv_draw_rect_dsc_t * draw_dsc);
    
protected:
    lv_obj_t*               createObject(lv_obj_t* parent) override;

protected:
    int                     _w, _h;
    void*                   _buffer;
};


////////////////////////////////////////////////////////////////////////////////////
// class CanvasVirtualWidget

class CanvasVirtualWidget : public Widget
{
protected:
    CanvasVirtualWidget(CanvasWidget* ref);

public:
    bool                    create(DisplayObject* parent) override;

    lv_obj_t *              getObject() override;
    bool                    getVisible() override;
    void                    setPosition(int x, int y) override;
    void                    setSize(int w, int h) override;
    void                    setVisible(bool show) override;

protected:
    CanvasWidget*           _ref;
    int                     _x, _y, _w, _h;
    bool                    _show;
};


////////////////////////////////////////////////////////////////////////////////////
// class CompassWidget

class CompassWidget : public CanvasVirtualWidget
{
public:
    CompassWidget(CanvasWidget* ref);

public:
    //
    void                    update() override;
    void                    draw(lv_coord_t heading, lv_coord_t bearing, int32_t method);

protected:
    void                    drawArrow(lv_coord_t cx, lv_coord_t cy, lv_coord_t radius, lv_coord_t angle, lv_color_t color);
};


////////////////////////////////////////////////////////////////////////////////////
// class VariometerWidget

class VariometerWidget : public CanvasVirtualWidget
{
public:
    VariometerWidget(CanvasWidget* ref);

public:
    //
    void                    update() override;
    void                    draw(float vario);

protected:
};


////////////////////////////////////////////////////////////////////////////////////
// class ThermalAssistant

class ThermalAssistant : public CanvasVirtualWidget
{
public:
    ThermalAssistant(CanvasWidget* ref);

public:
    //
    void                    update() override;

    void                    drawTrack();
    void                    drawCompass();
    void                    drawWindDirection();
    void                    drawFlight();

protected:
};



#if 0
typedef struct _annunciator
{
    uint8_t     status;
    time_t      time;
    float       power;

} annunciator_t;

typedef struct _box_style
{
    const char* title;
    const char* desc;
    uint32_t    style;  // box-style: border

    lv_font_t*  font_title;
    lv_font_t*  font_desc;
    lv_font_t*  font_value;


} box_style_t;

typedef struct _box_value
{
    uint8_t type; // value type
    union {
        int     i;
        long    l;
        float   f;
        double  d;
    } data;
} box_value_t;

lv_obj_t* widget_annunciator_create(lv_obj_t* parent, annunciator_t* data);
lv_obj_t* widget_text_box_create(lv_obj_t* parent, box_style_t* style, box_value_t* value);
lv_obj_t* widget_compas_create();
lv_obj_t* widget_varometer_create();
lv_obj_t* widget_vario_profile_create();
lv_obj_t* widget_altitude_profile_create();
lv_obj_t* widget_thermal_assistant_create();

void annunciator_set_status(lv_obj_t* obj, uint8_t status);
void annunciator_set_time(lv_obj_t* obj, time_t time);
void annunciator_set_battery_power(lv_obj_t* obj, float voltage);

void textbox_set_title(lv_obj_t* obj, const char* title);
void textbox_set_description(lv_obj_t* obj, const char* desc);
void textbox_set_integer(lv_obj_t*, long value);
void textbox_set_number(lv_obj_t*, float value);
void textbox_set_format();
void textbox_set_style();
void textbox_set_value_format();

void variometer_set_value(lv_obj_t* obj, float value);

void vario_profile_add_value(lv_obj_t* obj, float value);

void altitude_profile_add_value(lv_obj_t* obj, float value);

void thermal_assistant_add_position(lv_obj_t* obj, float lat, float lon, float speed);
#endif

#endif // __WIDGET_H__
