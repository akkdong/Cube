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



////////////////////////////////////////////////////////////////////////////////////
// class Widget

class Widget
{
public:
    Widget();

public:
    //
    operator lv_obj_t* () { return widget; }

    //
    virtual void            create(lv_obj_t* parent) = 0;
    virtual void            update() = 0;

    void                    set_position(int x, int y);
    void                    set_size(int w, int h);


    //
    static void             init();

protected:
    virtual void            onCreate();

protected:
    //
    lv_obj_t*               widget;

    //
    static lv_style_t*      def_style;

    static const lv_font_t* font_title;
    static const lv_font_t* font_subtitle;
    static const lv_font_t* font_body;
};


////////////////////////////////////////////////////////////////////////////////////
// class Annunciator

class Annunciator : public Widget
{
public:
    Annunciator();

public:
    //
    void                    create(lv_obj_t* parent) override;
    void                    update() override;

protected:
    virtual void            onCreate();

protected:
    lv_obj_t*               logo;
    lv_obj_t*               gps;        // invalid, unfixed, fixed
    lv_obj_t*               bluetooth;  // invalid, disconnected, connected
    lv_obj_t*               sdcard;     // invalid, no-card, ready, logging
    lv_obj_t*               volume;     // mute, low, middle, loud

    lv_obj_t*               power;      // ? icon, voltage, percentage
    lv_obj_t*               clock;      // [YYYY-MM-DD] HH-MM-DD
};


////////////////////////////////////////////////////////////////////////////////////
// class NumberBox

class NumberBox : public Widget
{
public:
    NumberBox();

public:
    //
    void                    create(lv_obj_t* parent) override;
    void                    update() override;

protected:
    lv_obj_t*               title;
    lv_obj_t*               desc;
    lv_obj_t*               content;
};


////////////////////////////////////////////////////////////////////////////////////
// class CanvasWidget

class CanvasWidget : public Widget
{
public:
    CanvasWidget(int width, int height) {
        size_t size = LV_CANVAS_BUF_SIZE_TRUE_COLOR(width, height);
        #ifdef ARDUINO
        canvas_buffer = ps_malloc(size);
        #else
        canvas_buffer = malloc(size);
        #endif
        //ASSERT(canvas_buffer);
    }

protected:
    void*                   canvas_buffer;
};


////////////////////////////////////////////////////////////////////////////////////
// class ThermalAssistant

class ThermalAssistant : public CanvasWidget
{
public:
    ThermalAssistant(int width, int height);

public:
    //
    void                    create(lv_obj_t* parent) override;
    void                    update() override;
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
