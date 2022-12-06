// flightWindow.cpp
//

#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "config.h"
#include "app.h"
#include "logger.h"

#include "DeviceRepository.h"
#include "flightWindow.h"
#include "settingWindow.h"
#include "widget.h"

#include "../assets/assets.h"


#define MAX_ANNUNCIATOR_HEIGHT      (48)

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
        0, 0, 300, 144 - 20, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        0, 144 + 10, 300, 144 - 20, 
        NumberBox::TRACK_HEADING, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        0, 288 + 20, 300, 144 - 20, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        500, 0, 300, 144 - 20, 
        NumberBox::ALTITUDE_BARO, 
    },
    {
        FlightWindow::WIDGET_BOX_5,
        500, 144 + 10, 300, 144 - 20, 
        NumberBox::SENSOR_PRESSURE, 
    },
    {
        FlightWindow::WIDGET_BOX_6,
        500, 288 + 20, 300, 144 - 20, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_COMPASS, 
        300 + 20, 0, 160, 160, 
        0
    },
    {
        FlightWindow::WIDGET_VARIOMETER,
        300 + 20 + 6, 200 - 40, 160 - 12, 232 + 40, 
        0
    },
};

WidgetLayout _layout_2[] =
{
    {
        FlightWindow::WIDGET_THERMAL_ASSISTANT, 
        0, 0, 512, 432, 
        0
    },
    {
        FlightWindow::WIDGET_COMPASS, 
        0, 0, 120, 120, 
        0
    },    
    {
        FlightWindow::WIDGET_BOX_1,
        512, 0, 288, 108, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        512, 108, 288, 108, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        512, 216, 288, 108, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        512, 324, 288, 108, 
        NumberBox::TRACK_HEADING, 
    },
};

WidgetLayout _layout_3[] =
{
    {
        FlightWindow::WIDGET_BOX_1,
        0, 0, 260, 108, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        0, 108, 260, 108, 
        NumberBox::ALTITUDE_BARO, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        0, 216, 260, 108, 
        NumberBox::ALTITUDE_AGL, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        0, 324, 260, 108, 
        NumberBox::DISTANCE_FLIGHT, 
    },

    {
        FlightWindow::WIDGET_BOX_5,
        260 + 10, 0, 260, 108, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_6,
        260 + 10, 108, 260, 108, 
        NumberBox::SPEED_VERTICAL, 
    },
    {
        FlightWindow::WIDGET_BOX_7,
        260 + 10, 216, 260, 108, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_BOX_8,
        260 + 10, 324, 260, 108, 
        NumberBox::DISTANCE_TAKEOFF, 
    },

    {
        FlightWindow::WIDGET_BOX_9,
        520 + 20, 0, 260, 108, 
        NumberBox::TRACK_HEADING, 
    },
    {
        FlightWindow::WIDGET_BOX_10,
        520 + 20, 108, 260, 108, 
        NumberBox::TIME_FLIGHT, 
    },
    {
        FlightWindow::WIDGET_BOX_11,
        520 + 20, 216, 260, 108, 
        NumberBox::TIME_CURRENT, 
    },
    {
        FlightWindow::WIDGET_BOX_12,
        520 + 20, 324, 260, 108 ,
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
    #ifdef ARDUINO
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFFFFFF), 0);
    #else
    // to test a widget with transparent background
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFFE0E0), 0);
    #endif

    //
    fontCustom = lv_imgfont_create(32, getCustomFont);
    fontCustom->fallback = &lv_font_montserrat_28;
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

    lv_obj_add_event_cb(_this, _onClickBackground, LV_EVENT_CLICKED, this);
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
    if (key == KEY_ENTER)
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

    // [LOGO] [BT] [GPS] [VOLUME] {SD} \t Voltage(or Percent) [BAT]
    char sz[32];
    sprintf(sz, " %s %s %s %s%s \t %.1fv %s", 
        APP_SYMBOL_LOGO_ANGRY_B,
        context->deviceState.statusBT ? APP_SYMBOL_BLUETOOTH_PAIRED : APP_SYMBOL_BLUETOOTH_PAIRED,
        context->deviceState.statusGPS ? APP_SYMBOL_GPS_FIXED : APP_SYMBOL_GPS_UNFIXED,
        APP_SYMBOL_VOLUME_HIGH,
        context->deviceState.statusSDCard == 0 ? "" : (context->deviceState.statusSDCard > 1  ? " " APP_SYMBOL_SDCARD_LOGGING : " " APP_SYMBOL_SDCARD_READY),
        context->deviceState.batteryPower,
        APP_SYMBOL_BATTERY_CHARGING);
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
    compass->draw(context->varioState.heading, /*context->varioState.bearing*/ -1, 1);
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
    assistant->drawFlight();
}

bool FlightWindow::getCustomFont(const lv_font_t * font, void * img_src, uint16_t len, uint32_t unicode, uint32_t unicode_next)
{
    const lv_img_dsc_t* img_icon = nullptr;

    switch (unicode)
    {
    case APP_ICON_BATTERY_CHARGING:     // 0xF001
        img_icon = &battery_charging;
        break;
    case APP_ICON_BATTERY_POWER:        // 0xF002
        img_icon = &battery_power;
        break;
    case APP_ICON_BLUETOOTH_PAIRED:     // 0xF003
        img_icon = &bluetooth_paired;
        break;
    case APP_ICON_BLUETOOTH_UNPAIRED:   // 0xF004
        img_icon = &bluetooth_unpaired;
        break;
    case APP_ICON_GPS_FIXED:            // 0xF005
        img_icon = &gps_fixed;
        break;
    case APP_ICON_GPS_UNFIXED:          // 0xF006
        img_icon = &gps_unfixed;
        break;
    case APP_ICON_LOGGING:              // 0xF007
        img_icon = &logging;
        break;
    case APP_ICON_LOGO:                 // 0xF008
        img_icon = &logo;
        break;
    case APP_ICON_LOGO_ANGRY_A:         // 0xF009
        img_icon = &logo_angry_a;
        break;
    case APP_ICON_LOGO_ANGRY_B:         // 0xF00A
        img_icon = &logo_angry_b;
        break;
    case APP_ICON_LOGO_O:               // 0xF00B
        img_icon = &logo_o;
        break;
    case APP_ICON_LOGO_ROUND:           // 0xF00C
        img_icon = &logo_round;
        break;
    case APP_ICON_SDCARD_READY:         // 0xF00D
        img_icon = &sdcard_ready;
        break;
    case APP_ICON_SDCARD_LOGGING:       // 0xF00E
        img_icon = &sdcard_logging;
        break;
    case APP_ICON_VOLUME_MUTE:          // 0xF00F
        img_icon = &volume_mute;
        break;
    case APP_ICON_VOLUME_LOW:           // 0xF010
        img_icon = &volume_low;
        break;
    case APP_ICON_VOLUME_HIGH:          // 0xF011
        img_icon = &volume_high;
        break;
    }

    if (img_icon != nullptr)
    {
        memcpy(img_src, img_icon, sizeof(lv_img_dsc_t));
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
