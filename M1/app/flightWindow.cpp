// flightWindow.cpp
//

#include <stdio.h>
#include <stdint.h>
#include "device_defines.h"
#include "app.h"
#include "bsp.h"
#include "logger.h"

#include "DeviceRepository.h"
#include "flightWindow.h"
#include "settingWindow.h"
#include "widget.h"

#define MAX_ANNUNCIATOR_HEIGHT      (32)

// 지원하는 widget과 widget의 properties
//
// widget
//  id
//  type
//  properties
//
//
// property
//  id
//  data-type
//  data-ptr
//

struct WidgetLayout {
    FlightWindow::WidgetID id;
    int x, y;
    int w, h;

    // widget parameters
    // ...
    uint32_t data;
};

WidgetLayout _layout_1[] =
{
    {
        FlightWindow::WIDGET_BOX_1,
        0, 0, 180, 76, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        0, 96 + 10, 180, 76, 
        NumberBox::TRACK_HEADING, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        0, 192 + 20, 180, 76, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        300, 0, 180, 76, 
        NumberBox::ALTITUDE_BARO, 
    },
    {
        FlightWindow::WIDGET_BOX_5,
        300, 96 + 10, 180, 76, 
        NumberBox::SENSOR_PRESSURE, 
    },
    {
        FlightWindow::WIDGET_BOX_6,
        300, 192 + 20, 180, 76, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_COMPASS, 
        190, 0, 100, 100, 
        0
    },
    {
        FlightWindow::WIDGET_VARIOMETER,
        200, 100, 80, 188, 
        0
    },
};

WidgetLayout _layout_2[] =
{
    {
        FlightWindow::WIDGET_THERMAL_ASSISTANT, 
        0, 0, 320, 288, 
        0
    },
    {
        FlightWindow::WIDGET_COMPASS, 
        0, 0, 80, 80, 
        0
    },    
    {
        FlightWindow::WIDGET_BOX_1,
        320, 0, 160, 72, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        320, 72, 160, 72, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        320, 144, 160, 72, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        320, 215, 160, 72, 
        NumberBox::TRACK_HEADING, 
    },
};

WidgetLayout _layout_3[] =
{
    {
        FlightWindow::WIDGET_BOX_1,
        0, 0, 160, 72, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        0, 72, 160, 72, 
        NumberBox::ALTITUDE_BARO, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        0, 144, 160, 72, 
        NumberBox::ALTITUDE_AGL, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        0, 215, 160, 72, 
        NumberBox::DISTANCE_FLIGHT, 
    },

    {
        FlightWindow::WIDGET_BOX_5,
        160, 0, 160, 72, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_6,
        160, 72, 160, 72, 
        NumberBox::SPEED_VERTICAL, 
    },
    {
        FlightWindow::WIDGET_BOX_7,
        160, 144, 160, 72, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_BOX_8,
        160, 215, 160, 72, 
        NumberBox::DISTANCE_TAKEOFF, 
    },

    {
        FlightWindow::WIDGET_BOX_9,
        320, 0, 160, 72, 
        NumberBox::TRACK_HEADING, 
    },
    {
        FlightWindow::WIDGET_BOX_10,
        320, 72, 160, 72, 
        NumberBox::TIME_FLIGHT, 
    },
    {
        FlightWindow::WIDGET_BOX_11,
        320, 144, 160, 72, 
        NumberBox::TIME_CURRENT, 
    },
    {
        FlightWindow::WIDGET_BOX_12,
        320, 215, 160, 72, 
        NumberBox::GLIDE_RATIO, 
    },        
};

struct LayoutInfo
{
    WidgetLayout*   layout;
    int             size;
};

LayoutInfo _layout[] = 
{
    { _layout_1, sizeof(_layout_1) / sizeof(_layout_1[0]) },
    { _layout_2, sizeof(_layout_2) / sizeof(_layout_2[0]) },
    { _layout_3, sizeof(_layout_3) / sizeof(_layout_3[0]) },
};



///////////////////////////////////////////////////////////////////////////////////
// class FlightWindow static members

const Widget::Type FlightWindow::widgetID2Type[] = {
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_1,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_2,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_3,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_4,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_5,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_6,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_7,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_8,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_9,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_10,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_11,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_12,
    Widget::WIDGET_PROFILE, // WIDGET_PROFILE_VARIO,
    Widget::WIDGET_PROFILE, // WIDGET_PROFILE_ALTITUDE,
    Widget::WIDGET_COMPASS, // WIDGET_COMPASS,
    Widget::WIDGET_VARIOMETER, // WIDGET_VARIOMETER,
    Widget::WIDGET_THERMAL_ASSISTANT, // WIDGET_THERMAL_ASSISTANT,    
};


///////////////////////////////////////////////////////////////////////////////////
// class FlightWindow

FlightWindow::FlightWindow()
    : annunciator()
    , bkgndCanvas(LCD_WIDTH, LCD_HEIGHT - MAX_ANNUNCIATOR_HEIGHT)
    , fontCustom(NULL)
    , activeLayout(0)
{
    memset(widgets, 0, sizeof(widgets));
}

FlightWindow::~FlightWindow()
{
    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
        delete widgets[i];
}

void FlightWindow::onCreate()
{
    // flight-window background
    #if defined(ARDUINO) || 1
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFFFFFF), 0);
    #else
    // to test a widget with transparent background
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFFE0E0), 0);
    #endif

    //
    fontCustom = lv_imgfont_create(18, getCustomFont);
    fontCustom->fallback = &lv_font_montserrat_16;
    fontCustom->base_line = 2;

    //
    annunciator.create(this);
    //ann.setConfig(...);
    annunciator.setUpdater(this);
    annunciator.setPosition(0, 0);
    annunciator.setSize(LCD_WIDTH, MAX_ANNUNCIATOR_HEIGHT);
    annunciator.setFont(fontCustom);
    annunciator.setStatus(" \uF002");

    bkgndCanvas.create(this);
    bkgndCanvas.setPosition(0, MAX_ANNUNCIATOR_HEIGHT);
    bkgndCanvas.setSize(LCD_WIDTH, LCD_HEIGHT - MAX_ANNUNCIATOR_HEIGHT);

    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
    {
        widgets[i] = createWidget(widgetID2Type[i], &bkgndCanvas);
        if (!widgets[i])
            continue;

        widgets[i]->setUpdater(this);
        widgets[i]->setVisible(false);
        
        //lv_obj_add_flag(widgets[i]->getObject(), LV_OBJ_FLAG_HIDDEN);
    }

    // load-layout
    // ...

    // layout-widgets
    #if 0
    //for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
    //    lv_obj_add_flag(widgets[i]->getObject(), LV_OBJ_FLAG_HIDDEN);

    for (int i = 0; i < sizeof(_layout_1) / sizeof(_layout_1[0]); i++)
    {
        WidgetLayout* layout = &_layout_1[i];
        Widget* widget = widgets[layout->id];
        if (!widget)
            continue;

        widget->setPosition(layout->x, layout->y);
        widget->setSize(layout->w, layout->h);

        switch (widgetID2Type[layout->id])
        {
        case Widget::WIDGET_NUMBER_BOX:
            ((NumberBox *)widget)->setType(layout->data);
            break;
        }

        lv_obj_clear_flag(widget->getObject(), LV_OBJ_FLAG_HIDDEN);
    }
    #endif
    layoutWidget(activeLayout);

//  bsp_regiter_keypad_receiver(_this);

    lv_obj_add_event_cb(_this, _onClickBackground, LV_EVENT_CLICKED, this);
//  lv_obj_add_event_cb(_this, _onClickBackground, LV_EVENT_PRESSED, this);
//  lv_obj_add_event_cb(_this, _onClickBackground, LV_EVENT_LONG_PRESSED, this);
//  lv_obj_add_event_cb(_this, _onClickBackground, LV_EVENT_RELEASED, this);
}

void FlightWindow::onActive()
{
}

void FlightWindow::onFallback(Result result, Window* from)
{
    LOGv("FlightWindow::onFallback(%d)", result);
}

void FlightWindow::onClose()
{
}

void FlightWindow::onKeyDown(uint16_t key)
{
    int layout = activeLayout;

    if (key == KEY_LEFT)
    {
        // move to previous layout
        layout = activeLayout - 1;
        if (layout < 0)
            layout = sizeof(_layout) / sizeof(_layout[0]) - 1;
    }
    else if (key == KEY_RIGHT)
    {
        // move to next layout
        layout = (activeLayout + 1) % (sizeof(_layout) / sizeof(_layout[0]));
    }

    if (layout != activeLayout)
    {
        activeLayout = layout;

        layoutWidget(activeLayout);
        update();
    }
}

void FlightWindow::onLongKeyDown(uint16_t key)
{
    if (key == KEY_DOWN)
    {
        // enter configuration menu
        Screen* screen = Screen::instance();
        screen->activateWindow(new SettingWindow);
    }
}

void FlightWindow::onKeyUp(uint16_t key)
{
}



void FlightWindow::update()
{
    annunciator.update();
    bkgndCanvas.update();

    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
    {
        if (widgets[i])
            widgets[i]->update();
    }
}

Widget* FlightWindow::createWidget(Widget::Type type, DisplayObject* parent)
{
    Widget* widget = NULL;

    switch (type)
    {
    case Widget::WIDGET_NUMBER_BOX:
        widget = new NumberBox;
        break;
    case Widget::WIDGET_PROFILE:
        widget = new ProfileWidget;
        break;
    case Widget::WIDGET_COMPASS:
        widget = new CompassWidget(&bkgndCanvas);
        break;
    case Widget::WIDGET_VARIOMETER:
        widget = new VariometerWidget(&bkgndCanvas);
        break;
    case Widget::WIDGET_THERMAL_ASSISTANT:
        widget = new ThermalAssistant(&bkgndCanvas);
        break;
    }

    if (widget && !widget->create(parent))
    {
        delete widget;
        widget = NULL;
    }

    return widget;
}

void FlightWindow::layoutWidget(int layout)
{
    LOGv("layout widgets #%d", layout);

    // hide all-widget
    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
    {
        if (!widgets[i])
            continue;

        widgets[i]->setVisible(false); // lv_obj_add_flag(widgets[i]->getObject(), LV_OBJ_FLAG_HIDDEN);
    }

    // 
    LayoutInfo* info = &_layout[layout];
    for (int i = 0; i < info->size; i++)
    {
        WidgetLayout* layout = &info->layout[i];
        Widget* widget = widgets[layout->id];
        if (!widget)
            continue;

        widget->setPosition(layout->x, layout->y);
        widget->setSize(layout->w, layout->h);

        switch (widgetID2Type[layout->id])
        {
        case Widget::WIDGET_NUMBER_BOX:
            ((NumberBox *)widget)->setType(layout->data);
            break;
        case Widget::WIDGET_PROFILE:
            ((ProfileWidget *)widget)->setDataType((ProfileWidget::DataType)layout->data);
            break;
        }

        widget->setVisible(true); // lv_obj_clear_flag(widget->getObject(), LV_OBJ_FLAG_HIDDEN);
    }    
}

//
void FlightWindow::onUpdate(Annunciator* ann)
{
    DeviceContext* context = DeviceRepository::instance().getContext();

    //
    char sz[32];
    // \uF001 logo
    // \uF002 BT (?)
    // \uF003 GPS (fixed)
    // \uF004 SOUND (on)
    //
    // + BT (ready, paired), GPS (unxied), SOUND (mute:off, low, mid, loud)
    // + SD (N/A:empty, valid, logging)
    // + FLIGHT-MODE (?)
    //
    // \uF001 -> \xEF\x80\x81 -> APP_SYMBOL_LOGO
    #define APP_SYMBOL_LOG      "\xEF\x80\x81"
    #define APP_SYMBOL_BT       "\xEF\x80\x82"
    #define APP_SYMBOL_GPS      "\xEF\x80\x83"
    #define APP_SYMBOL_SOUND    "\xEF\x80\x84"

    sprintf(sz, " \uF001%s%s%s \t %.1fv %s", 
        context->deviceState.statusBT ? " " APP_SYMBOL_BT : "",
        context->deviceState.statusGPS? " " APP_SYMBOL_GPS : "",
        "",
        context->deviceState.batteryPower,
        LV_SYMBOL_BATTERY_FULL);
    ann->setStatus(sz);

    // time_t t = time(NULL) /*+ 9 * 60 * 60*/;
    getTimeString(sz, time(NULL), true);
    ann->setClock(sz);
}

void FlightWindow::onUpdate(NumberBox* box)
{
    //app_conf_t* conf = app_get_conf();
    DeviceContext* context = DeviceRepository::instance().getContext();

    char value[32];
    strcpy(value, "");

    switch (box->getType())
    {
    case NumberBox::ALTITUDE_GROUND:
        sprintf(value, "%.0f", context->varioState.altitudeGPS);
        break;
    case NumberBox::ALTITUDE_BARO:
        sprintf(value, "%.0f", context->varioState.altitudeBaro);
        break;
    case NumberBox::ALTITUDE_AGL:
        sprintf(value, "%.0f", context->varioState.altitudeAGL);
        break;
    case NumberBox::ALTITUDE_PROFILE:
        break;
    case NumberBox::SPEED_GROUND:
        sprintf(value, "%.0f", context->varioState.speedGround);
        break;
    case NumberBox::SPEED_AIR:
        break;
    case NumberBox::SPEED_VERTICAL:
        sprintf(value, "%.1f", context->varioState.speedVertActive);
        break;
    case NumberBox::SPEED_VERTICAL_LAZY:
        sprintf(value, "%.1f", context->varioState.speedVertLazy);
        break;
    case NumberBox::TRACK_HEADING:
        sprintf(value, "%d", context->varioState.heading);
        break;
    case NumberBox::TARCK_BEARING:
        sprintf(value, "%d", context->flightState.bearingNextPoint);
        break;
    case NumberBox::TIME_FLIGHT:
        getElapsedTimeString(value, context->flightState.flightTime);
        break;
    case NumberBox::TIME_CURRENT:
        if (context->varioState.timeCurrent == 0)
            strcpy(value, "--:--");
        else
            getTimeString(value, time(NULL));
        break;
    case NumberBox::TIME_TO_NEXT_WAYPOINT:
        break;
    case NumberBox::TIME_REMAIN:
        break;
    case NumberBox::DISTANCE_TAKEOFF:
        sprintf(value, "%.1f", context->flightState.distTakeoff);
        break;
    case NumberBox::DISTANCE_LANDING:
        break;
    case NumberBox::DISTANCE_NEXT_WAYPOINT:
        sprintf(value, "%.1f", context->flightState.distNextPoint);
        break;
    case NumberBox::DISTANCE_FLIGHT:
        sprintf(value, "%.1f", context->flightState.distFlight);
        break;
    case NumberBox::GLIDE_RATIO:
        sprintf(value, "%.1f", context->flightState.glideRatio);
        break;
    case NumberBox::COMPASS:
        sprintf(value, "%.0f", context->varioState.heading);
        break;
    case NumberBox::VSPEED_BAR:
    case NumberBox::VSPEED_PROFILE:
    case NumberBox::TRACK_FLIGHT:
        break;
    case NumberBox::SENSOR_PRESSURE:
        sprintf(value, "%.0f", context->varioState.pressure);
        break;
    case NumberBox::SENSOR_TEMPERATURE:
        sprintf(value, "%.1f", context->varioState.temperature);
        break;
    case NumberBox::SENSOR_HUMIDITY:
        break;
    default:
        break;
    }

    box->setValue(value);
}

void FlightWindow::onUpdate(ProfileWidget *)
{
}

void FlightWindow::onUpdate(CompassWidget* compass)
{
    if (!compass->getVisible())
        return;

    //app_conf_t* conf = app_get_conf();
    DeviceContext* context = DeviceRepository::instance().getContext();
    compass->draw(context->varioState.heading, /*context->varioState.bearing*/ 240, 1);
}

void FlightWindow::onUpdate(VariometerWidget* variometer)
{
    if (!variometer->getVisible())
        return;

    //app_conf_t* conf = app_get_conf();
    DeviceContext* context = DeviceRepository::instance().getContext();
    variometer->draw(context->varioState.speedVertLazy);
}

void FlightWindow::onUpdate(ThermalAssistant* assistant)
{
    if (!assistant->getVisible())
        return;

    DeviceContext* context = DeviceRepository::instance().getContext();
    assistant->drawTrack(context->flightState, context->varioState.heading);
    assistant->drawCompass();
    assistant->drawWindDirection();
    assistant->drawFlight(context->varioState.heading, -1, 1);
}


extern const lv_img_dsc_t paper_plane;
extern const lv_img_dsc_t bluetooth;
extern const lv_img_dsc_t map_marker;
extern const lv_img_dsc_t volume;

bool FlightWindow::getCustomFont(const lv_font_t * font, void * img_src, uint16_t len, uint32_t unicode, uint32_t unicode_next)
{
    if (unicode == 0xF001)
    {
        memcpy(img_src, &paper_plane, sizeof(lv_img_dsc_t));
        return true;
    }
    else if (unicode == 0xF002)
    {
        memcpy(img_src, &bluetooth, sizeof(lv_img_dsc_t));
        return true;
    }
    else if (unicode == 0xF003)
    {
        memcpy(img_src, &map_marker, sizeof(lv_img_dsc_t));
        return true;
    }
    else if (unicode == 0xF004)
    {
        memcpy(img_src, &volume, sizeof(lv_img_dsc_t));
        return true;
    }

    return false;
}

void FlightWindow::_onClickBackground(lv_event_t* event)
{
    LOGi("Page Event: %d", event->code);
}

const char * FlightWindow::getElapsedTimeString(char* str, time_t t)
{
    time_t temp = t % 3600;
    time_t h = t / 3600;
    time_t m = temp / 60;
    time_t s = temp % 60;

    if (h != 0)
        sprintf(str, "%d:%02d:%02d", h, m, s);
    else
        sprintf(str, "%02d:%02d", m, s);

    return str;
}

const char * FlightWindow::getTimeString(char* str, time_t t, bool includeSecond)
{
	struct tm * _tm = localtime(&t);    
    if (includeSecond)
	    sprintf(str, "%02d:%02d:%02d", _tm->tm_hour, _tm->tm_min,_tm->tm_sec);
    else
        sprintf(str, "%02d:%02d", _tm->tm_hour, _tm->tm_min);

    return str;
}
