// flightWindow.cpp
//

#include <stdio.h>
#include <stdint.h>
#include "device_defines.h"
#include "lv_app.h"
#include "logger.h"

#include "flightWindow.h"
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
        0, 0, 180, 96, 
        NumberBox::ALTITUDE_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_2,
        0, 96, 180, 96, 
        NumberBox::TRACK_HEADING, 
    },
    {
        FlightWindow::WIDGET_BOX_3,
        0, 192, 180, 96, 
        NumberBox::SPEED_GROUND, 
    },
    {
        FlightWindow::WIDGET_BOX_4,
        300, 0, 180, 96, 
        NumberBox::ALTITUDE_BARO, 
    },
    {
        FlightWindow::WIDGET_BOX_5,
        300, 96, 180, 96, 
        NumberBox::SENSOR_PRESSURE, 
    },
    {
        FlightWindow::WIDGET_BOX_6,
        300, 192, 180, 96, 
        NumberBox::SPEED_VERTICAL_LAZY, 
    },
    {
        FlightWindow::WIDGET_COMPASS, 
        180, 0, 120, 120, 
        0
    },
    {
        FlightWindow::WIDGET_VARIOMETER,
        200, 120, 80, 168, 
        0
    },
};

WidgetLayout _layout_2[] =
{

};

WidgetLayout _layout_3[] =
{

};


///////////////////////////////////////////////////////////////////////////////////
// class FlightWindow static members

const Widget::Type FlightWindow::widgetType[] = {
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_1,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_2,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_3,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_4,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_5,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_6,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_7,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_8,
    Widget::WIDGET_NUMBER_BOX, // WIDGET_BOX_9,
    Widget::WIDGET_COMPASS, // WIDGET_COMPASS,
    Widget::WIDGET_VARIOMETER, // WIDGET_VARIOMETER,
    Widget::WIDGET_PROFILE_VARIO, // WIDGET_PROFILE_VARIO,
    Widget::WIDGET_PROFILE_ALTITUDE, // WIDGET_PROFILE_ALTITUDE,
    Widget::WIDGET_THERMAL_ASSISTANT, // WIDGET_THERMAL_ASSISTANT,    
};


///////////////////////////////////////////////////////////////////////////////////
// class FlightWindow

FlightWindow::FlightWindow()
    : annunciator()
    , bkgndCanvas(LCD_WIDTH, LCD_HEIGHT - MAX_ANNUNCIATOR_HEIGHT)
    , fontCustom(NULL)
{
    memset(widgets, 0, sizeof(widgets));
}

FlightWindow::~FlightWindow()
{
    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
        delete widgets[i];
}

void FlightWindow::onCreate(DisplayObject* parent)
{
    //
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFFFFFF), 0);
    //lv_obj_set_style_bg_opa(_this, LV_OPA_TRANSP, 0);

    //
    fontCustom = lv_imgfont_create(16, getCustomFont);
    fontCustom->fallback = &lv_font_montserrat_16;

    //
    annunciator.create(this);
    //ann.setConfig(...);
    annunciator.setUpdater(this);
    annunciator.setPosition(0, 0);
    annunciator.setSize(LCD_WIDTH, MAX_ANNUNCIATOR_HEIGHT);
    annunciator.setFont(fontCustom);
    annunciator.setStatus("\uF001 \uF002 \uF003 \uF004 \t 100%");

    bkgndCanvas.create(this);
    bkgndCanvas.setPosition(0, MAX_ANNUNCIATOR_HEIGHT);
    bkgndCanvas.setSize(LCD_WIDTH, LCD_HEIGHT - MAX_ANNUNCIATOR_HEIGHT);

    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
    {
        widgets[i] = createWidget(widgetType[i]);
        if (!widgets[i])
            continue;
            
        widgets[i]->setUpdater(this);
        widgets[i]->create(&bkgndCanvas);
        
        lv_obj_add_flag(widgets[i]->getObject(), LV_OBJ_FLAG_HIDDEN);
    }

    // load-layout
    // ...

    // layout-widgets
    //for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
    //    lv_obj_add_flag(widgets[i]->getObject(), LV_OBJ_FLAG_HIDDEN);

    for (int i = 0; i < sizeof(_layout_1) / sizeof(_layout_1[0]); i++)
    {
        WidgetLayout* layout = &_layout_1[i];
        Widget* widget = widgets[layout->id];

        widget->setPosition(layout->x, layout->y /*+ MAX_ANNUNCIATOR_HEIGHT*/);
        widget->setSize(layout->w, layout->h);

        switch (widgetType[layout->id])
        {
        case Widget::WIDGET_NUMBER_BOX:
            ((NumberBox *)widget)->setType(layout->data);
            break;
        }

        lv_obj_clear_flag(widget->getObject(), LV_OBJ_FLAG_HIDDEN);
    }
}

void FlightWindow::onActive()
{

}

void FlightWindow::onClose()
{

}

void FlightWindow::update()
{
    annunciator.update();
    bkgndCanvas.update();

    for (int i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++)
        widgets[i]->update();
}

Widget* FlightWindow::createWidget(Widget::Type type)
{
    switch (type)
    {
    case Widget::WIDGET_NUMBER_BOX:
        return new NumberBox;
    case Widget::WIDGET_COMPASS:
        return new CompassWidget(&bkgndCanvas);
    case Widget::WIDGET_VARIOMETER:
        return new VariometerWidget(&bkgndCanvas);
    case Widget::WIDGET_PROFILE_VARIO:
        return new NumberBox;
    case Widget::WIDGET_PROFILE_ALTITUDE:
        return new NumberBox;
    case Widget::WIDGET_THERMAL_ASSISTANT:
        return new ThermalAssistant(&bkgndCanvas);
    }

    return NULL;
}

//
void FlightWindow::onUpdate(Annunciator* ann)
{
    //
    time_t t = time(NULL) /*+ 9 * 60 * 60*/;
    struct tm* _tm = localtime(&t);
    
    char sz[32];
    sprintf(sz, "%d:%02d:%02d", _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
    ann->setClock(sz);
}

void FlightWindow::onUpdate(NumberBox* box)
{
    app_conf_t* conf = app_get_conf();
    char value[32];

    switch (box->getType())
    {
    case NumberBox::ALTITUDE_GROUND:
        sprintf(value, "%.0f", conf->altitudeGPS);
        break;
    case NumberBox::ALTITUDE_BARO:
        sprintf(value, "%.0f", conf->altitudeBaro);
        break;
    case NumberBox::ALTITUDE_AGL:
        sprintf(value, "%.0f", conf->altitudeAGL);
        break;
    case NumberBox::ALTITUDE_PROFILE:
        break;
    case NumberBox::SPEED_GROUND:
        sprintf(value, "%.0f", conf->speedGround);
        break;
    case NumberBox::SPEED_AIR:
        break;
    case NumberBox::SPEED_VERTICAL:
        sprintf(value, "%.1f", conf->speedVertActive);
        break;
    case NumberBox::SPEED_VERTICAL_LAZY:
        sprintf(value, "%.1f", conf->speedVertLazy);
        break;
    case NumberBox::TRACK_HEADING:
        sprintf(value, "%d", conf->heading);
        break;
    case NumberBox::TARCK_BEARING:
        sprintf(value, "%d", conf->bearing);
        break;
    case NumberBox::TIME_FLIGHT:
        if (conf->timeFly > 0)
        {
            time_t temp = conf->timeFly % 3600;
            time_t h = conf->timeFly / 3600;
            time_t m = temp / 60;
            time_t s = temp % 60;
            if (h != 0)
                sprintf(value, "%d:%02d:%02d", h, m, s);
            else
                sprintf(value, "%02d:%02d", m, s);
        }
        break;
    case NumberBox::TIME_CURRENT:
    case NumberBox::TIME_TO_NEXT_WAYPOINT:
    case NumberBox::TIME_REMAIN:
    case NumberBox::DISTANCE_TAKEOFF:
    case NumberBox::DISTANCE_LANDING:
    case NumberBox::DISTANCE_NEXT_WAYPOINT:
    case NumberBox::DISTANCE_FLIGHT:
        break;
    case NumberBox::GLIDE_RATIO:
        sprintf(value, "%.1", conf->glideRatio);
        break;
    case NumberBox::COMPASS:
        sprintf(value, "%.0f", conf->heading);
        break;
    case NumberBox::VSPEED_BAR:
    case NumberBox::VSPEED_PROFILE:
    case NumberBox::TRACK_FLIGHT:
        break;
    case NumberBox::SENSOR_PRESSURE:
        sprintf(value, "%.0f", conf->pressure);
        break;
    case NumberBox::SENSOR_TEMPERATURE:
        sprintf(value, "%.1f", conf->temperature);
        break;
    case NumberBox::SENSOR_HUMIDITY:
        break;
    default:
        strcpy(value, "");
        break;
    }

    box->setValue(value);
}

void FlightWindow::onUpdate(CompassWidget* compass)
{
    app_conf_t* conf = app_get_conf();
    compass->draw(conf->heading, conf->bearing, 1);
}

void FlightWindow::onUpdate(VariometerWidget* variometer)
{
    app_conf_t* conf = app_get_conf();
    variometer->draw(conf->speedVertLazy);
}

void FlightWindow::onUpdate(VarioProfile *)
{
}

void FlightWindow::onUpdate(AltitudeProfile *)
{
}

void FlightWindow::onUpdate(ThermalAssistant *)
{
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