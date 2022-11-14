// widget.cpp
//

#include <stdint.h>
#include <math.h>
#include <algorithm>

#include "device_defines.h"
#include "logger.h"
#include "widget.h"


////////////////////////////////////////////////////////////////////////////////////
// class Widget

lv_style_t* Widget::style_default = NULL;


Widget::Widget() : updater(NULL)
{
    if (!style_default)
        Widget::init();
}

Widget::~Widget()
{
    // nop
}

bool Widget::create(DisplayObject* parent)
{
    if (!DisplayObject::create(parent))
        return false;

    if (style_default)
        lv_obj_add_style(_this, style_default, 0);

    return true;
}

void Widget::init()
{
    //
    style_default = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
    if (style_default != NULL)
    {
        lv_style_init(style_default);
        lv_style_set_pad_hor(style_default, 4);
        lv_style_set_pad_ver(style_default, 2);
        lv_style_set_radius(style_default, 0/*4*/);
        lv_style_set_border_width(style_default, 0/*1*/);
        lv_style_set_border_side(style_default, LV_BORDER_SIDE_NONE);

        lv_style_set_bg_color(style_default, lv_color_hex(0xE0E0E0));
        lv_style_set_bg_opa(style_default, LV_OPA_0);
    }
}



////////////////////////////////////////////////////////////////////////////////////
// class Annunciator

Annunciator::Annunciator()
    : status(NULL)
    , clock(NULL)
{

}

void Annunciator::onCreate()
{
    //
    lv_obj_t* ann = this->getObject();
    lv_obj_set_style_pad_hor(ann, 4, 0);
    lv_obj_set_style_pad_ver(ann, 0, 0);
    lv_obj_set_style_radius(ann, 0, 0);
    lv_obj_set_style_border_width(ann, 2, 0);
    lv_obj_set_style_border_color(ann, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_color(ann, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_opa(ann, LV_OPA_100, 0);

    // create sub-objects
    status = lv_label_create(this->getObject());
    if (status)
    {
        lv_obj_set_style_text_font(status, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(status, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(status, LV_ALIGN_LEFT_MID, 0, 0);
        lv_label_set_text(status, "");
    }

    clock = lv_label_create(this->getObject());
    if (clock)
    {
        lv_obj_set_style_text_font(clock, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(clock, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(clock, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_label_set_text(clock, "00:00:00");
    }
}

void Annunciator::update()
{
    if (updater)
        updater->onUpdate(this);
}

void Annunciator::setStatus(const char* statusText)
{
    if (status)
        lv_label_set_text(status, statusText);
}

void Annunciator::setClock(const char* timeText)
{
    if (clock)
        lv_label_set_text(clock, timeText);
}

void Annunciator::setFont(const lv_font_t* font)
{
    if (status)
        lv_obj_set_style_text_font(status, font, 0);
}




////////////////////////////////////////////////////////////////////////////////////
// class NumberBox

const lv_font_t* NumberBox::font_title = &lv_font_montserrat_14;
const lv_font_t* NumberBox::font_subtitle = &lv_font_montserrat_12;
const lv_font_t* NumberBox::font_body = &lv_font_montserrat_48;


NumberBox::NumberBox()
    : titleObj(NULL)
    , descObj(NULL)
    , valueObj(NULL)
    , borderType(BORDER_NONE)
    , titleText(NULL)
    , descText(NULL)
    , boxType(UNDEF)
{

}

void NumberBox::update()
{
    if (updater)
        updater->onUpdate(this);
}

void NumberBox::onCreate()
{
    /*
    // update properties
    lv_obj_clear_flag(_this, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    */

    setType(UNDEF);
}

void NumberBox::setValue(const char* value)
{
    if (!valueObj)
    {
        valueObj = lv_label_create(this->getObject());
        if (valueObj)
        {
            lv_obj_align(valueObj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
            lv_obj_set_style_text_font(valueObj, font_body, 0);
            lv_label_set_text(valueObj, "");
        }
    }

    if (valueObj)
        lv_label_set_text(valueObj, value ? value : "");
}

void NumberBox::setType(uint32_t type)
{
    //LOGv("NumberBox::setType(%d -> %d)\n", this->boxType, type);
    switch (type)
    {
    case ALTITUDE_GROUND:
    case ALTITUDE_BARO:
    case ALTITUDE_AGL:
    case ALTITUDE_PROFILE:
    case SPEED_GROUND:
    case SPEED_AIR:
    case SPEED_VERTICAL:
    case SPEED_VERTICAL_LAZY:
    case TRACK_HEADING:
    case TARCK_BEARING:
    case TIME_FLIGHT:
    case TIME_CURRENT:
    case TIME_TO_NEXT_WAYPOINT:
    case TIME_REMAIN:
    case DISTANCE_TAKEOFF:
    case DISTANCE_LANDING:
    case DISTANCE_NEXT_WAYPOINT:
    case DISTANCE_FLIGHT:
    case GLIDE_RATIO:
    case COMPASS:
    case VSPEED_BAR:
    case VSPEED_PROFILE:
    case TRACK_FLIGHT:
    case SENSOR_PRESSURE:
    case SENSOR_TEMPERATURE:
    case SENSOR_HUMIDITY:
        this->boxType = (BoxType)type;
        break;
    }

    setTitle(getTitle(this->boxType));
    setDescription(getDescription(this->boxType));
    setValue("");
}

const char* NumberBox::getTitle(BoxType type)
{
    switch (type)
    {
    case ALTITUDE_GROUND:
        return "Altitude G.";
    case ALTITUDE_BARO:
        return "Altitude Baro";
    case ALTITUDE_AGL:
        return "AGL";
    case ALTITUDE_PROFILE:
        return NULL;
    case SPEED_GROUND:
        return "Speed G.";
    case SPEED_AIR:
        return "Speed A.";
    case SPEED_VERTICAL:
        return "Speed V.";
    case SPEED_VERTICAL_LAZY:
        return "Speed V.";
    case TRACK_HEADING:
        return "Track Hd.";
    case TARCK_BEARING:
        return "Track Br.";
    case TIME_FLIGHT:
        return "Time Fl.";
    case TIME_CURRENT:
        return "Time Cur.";
    case TIME_TO_NEXT_WAYPOINT:
        return "Time to Next";
    case TIME_REMAIN:
        return "Time Remain";
    case DISTANCE_TAKEOFF:
        return "Dist Toff";
    case DISTANCE_LANDING:
        return "Dist Land";
    case DISTANCE_NEXT_WAYPOINT:
        return "Dist Next";
    case DISTANCE_FLIGHT:
        return "Dist Fl.";
    case GLIDE_RATIO:
        return "L/D";
    case COMPASS:
        return NULL;
    case VSPEED_BAR:
        return NULL;
    case VSPEED_PROFILE:
        return NULL;
    case TRACK_FLIGHT:
        return NULL;
    case SENSOR_PRESSURE:
        return "Pressure";
    case SENSOR_TEMPERATURE:
        return "Temperature";
    case SENSOR_HUMIDITY:
        return "Humidity";
    default:
        return NULL;
    }
}

const char* NumberBox::getDescription(BoxType type)
{
    switch (type)
    {
    case ALTITUDE_GROUND:
    case ALTITUDE_BARO:
    case ALTITUDE_AGL:
        return "m";
    case ALTITUDE_PROFILE:
        return NULL;
    case SPEED_GROUND:
        return "km/h";
    case SPEED_AIR:
        return "km/h";
    case SPEED_VERTICAL:
        return "m/s";
    case SPEED_VERTICAL_LAZY:
        return "m/s";
    case TRACK_HEADING:
        return "Deg";
    case TARCK_BEARING:
        return "Deg";
    case TIME_FLIGHT:
        return "[h:]m:s";
    case TIME_CURRENT:
        return "h:m:s";
    case TIME_TO_NEXT_WAYPOINT:
        return "m:s";
    case TIME_REMAIN:
        return "[h:]m:s";
    case DISTANCE_TAKEOFF:
        return "m";
    case DISTANCE_LANDING:
        return "m";
    case DISTANCE_NEXT_WAYPOINT:
        return "m";
    case DISTANCE_FLIGHT:
        return "m";
    case GLIDE_RATIO:
        return NULL;
    case COMPASS:
        return NULL;
    case VSPEED_BAR:
        return NULL;
    case VSPEED_PROFILE:
        return NULL;
    case TRACK_FLIGHT:
        return NULL;
    case SENSOR_PRESSURE:
        return "Pa";
    case SENSOR_TEMPERATURE:
        return "C";
    case SENSOR_HUMIDITY:
        return "%%";
    default:
        return NULL;
    }
}

void NumberBox::setTitle(const char* title)
{
    //LOGv("NumberBox::setTitle(%s)\n", title);
    if (title && title[0])
    {
        if (!titleObj)
            titleObj = lv_label_create(this->getObject());

        if (titleObj)
        {
            lv_label_set_text(titleObj, title);
            lv_obj_set_style_text_font(titleObj, font_title, 0);
            lv_obj_align(titleObj, LV_ALIGN_TOP_LEFT, 0, 0);
        }
    }
    else
    {
        if (titleObj)
        {
            lv_obj_del(titleObj);
            titleObj = NULL;
        }
    }
}

void NumberBox::setDescription(const char* desc)
{
    //LOGv("NumberBox::setDescription(%s)\n", desc);
    if (desc && desc[0])
    {
        if (!descObj)
            descObj = lv_label_create(this->getObject());

        if (descObj)
        {
            lv_label_set_text(descObj, desc);
            lv_obj_set_style_text_font(descObj, font_subtitle, 0);
            lv_obj_align(descObj, LV_ALIGN_TOP_RIGHT, 0, 0);
        }
    }
    else
    {
        if (descObj)
        {
            lv_obj_del(descObj);
            descObj = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
// class CanvasWidget

CanvasWidget::CanvasWidget(int width, int height) 
    : _w(width)
    , _h(height)
    , _buffer(NULL)
    , _canvas(NULL)
{
    size_t size = LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(width, height);
    #ifdef ARDUINO
    _buffer = ps_malloc(size);
    #else
    _buffer = malloc(size);
    #endif
    //ASSERT(canvas_buffer);
}

CanvasWidget::~CanvasWidget()
{
    #ifdef ARDUINO
    free(_buffer);
    #else
    free(_buffer);
    #endif
}

void CanvasWidget::onCreate()
{
    //
    lv_obj_set_style_pad_hor(_this, 0, 0);
    lv_obj_set_style_pad_ver(_this, 0, 0);
    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(_this, LV_OPA_TRANSP, 0);

    //
    _canvas = lv_canvas_create(_this);
    if (!_canvas)
        return;

    lv_obj_set_align(_canvas, LV_ALIGN_CENTER);
    lv_canvas_set_buffer(_canvas, _buffer, _w, _h, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_canvas_fill_bg(_canvas, lv_color_hex(0xFFFFFF), LV_OPA_TRANSP);
}

void CanvasWidget::update()
{
    // erase backgournd
    lv_canvas_fill_bg(_canvas, lv_color_hex(0xFFFFFF), LV_OPA_TRANSP);

    // each canvas-virtual-object will draw on this canvas
    // ...
}

void CanvasWidget::drawRect(lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, const lv_draw_rect_dsc_t * draw_dsc)
{
    if (_canvas)
        lv_canvas_draw_rect(_canvas, x, y, w, h, draw_dsc);
}

void CanvasWidget::drawText(lv_coord_t x, lv_coord_t y, lv_coord_t max_w, lv_draw_label_dsc_t * draw_dsc, const char * txt)
{
    if (_canvas)
        lv_canvas_draw_text(_canvas, x, y, max_w, draw_dsc, txt);
}

void CanvasWidget::drawImage(lv_coord_t x, lv_coord_t y, const void * src, const lv_draw_img_dsc_t * draw_dsc)
{
    if (_canvas)
        lv_canvas_draw_img(_canvas, x, y, src, draw_dsc);
}

void CanvasWidget::drawLine(const lv_point_t points[], uint32_t point_cnt, const lv_draw_line_dsc_t * draw_dsc)
{
    if (_canvas)
        lv_canvas_draw_line(_canvas, points, point_cnt, draw_dsc);
}

void CanvasWidget::drawArc(lv_coord_t x, lv_coord_t y, lv_coord_t r, int32_t start_angle, int32_t end_angle, const lv_draw_arc_dsc_t * draw_dsc)
{
    if (_canvas)
        lv_canvas_draw_arc(_canvas, x, y, r, start_angle, end_angle, draw_dsc);
}

void CanvasWidget::drawPolygon(const lv_point_t points[], uint32_t point_cnt, const lv_draw_rect_dsc_t * draw_dsc)
{
    if (_canvas)
        lv_canvas_draw_polygon(_canvas, points, point_cnt, draw_dsc);
}



////////////////////////////////////////////////////////////////////////////////////
// class CanvasVirtualWidget


CanvasVirtualWidget::CanvasVirtualWidget(CanvasWidget* ref)
    : _ref(ref)
    , _x(0)
    , _y(0)
    , _w(0)
    , _h(0)
    , _show(true)
{

}

bool CanvasVirtualWidget::create(DisplayObject* parent)
{
    if (_ref && _ref->getObject())
    {
        //if (lv_obj_get_parent(_ref->getObject()) == parent->getObject())
        {
            // skip object-creation, just call onCreate only
            this->onCreate();

            return true;
        }
    }

    return false;
}

lv_obj_t* CanvasVirtualWidget::getObject()
{
    //return _ref ? _ref->getObject() : NULL;
    return NULL;
}

bool CanvasVirtualWidget::getVisible()
{
    return _ref && _show ? true : false;
}

void CanvasVirtualWidget::setPosition(int x, int y)
{
    _x = x;
    _y = y;
}

void CanvasVirtualWidget::setSize(int w, int h)
{
    _w = w;
    _h = h;
}

void CanvasVirtualWidget::setVisible(bool show)
{
    _show = show;
}



////////////////////////////////////////////////////////////////////////////////////
// class CompassWidget

CompassWidget::CompassWidget(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
{

}

void CompassWidget::onCreate()
{

}

void CompassWidget::update()
{
    // draw on reference-canvas
    if (updater)
        updater->onUpdate(this);
}

void CompassWidget::draw(lv_coord_t heading, lv_coord_t bearing, int32_t method)
{
    //
    lv_coord_t base_x = this->_w / 2 + _x;
    lv_coord_t base_y = this->_h / 2 + _y;
    lv_coord_t angle, up = 0;
    lv_coord_t COMPASS_RADIUS = this->_w / 2 - 6;
    
    if (method == 1)
        up = heading;
    else if (method == 2 && bearing >= 0) // bearing < 0 : n/a
        up = bearing;

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_line_dsc_t line_dsc;
    lv_point_t points[3];

    lv_draw_arc_dsc_init(&arc_dsc);
    lv_draw_line_dsc_init(&line_dsc);
    lv_draw_rect_dsc_init(&rect_dsc);

    // erase background
    //lv_canvas_fill_bg(canvas, lv_color_hex(0xFFFFFF), LV_OPA_100);

    // draw circle
    arc_dsc.color = lv_color_hex(0x000000);
    arc_dsc.width = 1;
    arc_dsc.start_angle = 0;
    arc_dsc.end_angle = 360;
    arc_dsc.img_src = NULL;
    arc_dsc.opa = LV_OPA_40;
    arc_dsc.blend_mode = LV_BLEND_MODE_NORMAL;
    arc_dsc.rounded = 0;
    _ref->drawArc(base_x, base_y, COMPASS_RADIUS + 2, 0, 360, &arc_dsc);

    // draw north-triangle
    angle = 0 - up; // real-north = rotate counterclockwise

    points[0].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin(angle * DEG_TO_RAD));
    points[0].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos(angle * DEG_TO_RAD));
    points[1].x = base_x + (lv_coord_t)(COMPASS_RADIUS / 3 * sin((angle + 180) * DEG_TO_RAD));
    points[1].y = base_y - (lv_coord_t)(COMPASS_RADIUS / 3 * cos((angle + 180) * DEG_TO_RAD));
    points[2].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle - 140) * DEG_TO_RAD));
    points[2].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle - 140) * DEG_TO_RAD));

    rect_dsc.bg_opa = LV_OPA_60;
    rect_dsc.bg_color = lv_color_hex(0xFF0000);
    //rect_dsc.border_width = 1;
    //rect_dsc.border_color = lv_color_hex(0x000000);
    //rect_dsc.border_opa = LV_OPA_20;
    _ref->drawPolygon(points, 3, &rect_dsc);

    points[2].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle + 140) * DEG_TO_RAD));
    points[2].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle + 140) * DEG_TO_RAD));

    rect_dsc.bg_opa = LV_OPA_80;
    _ref->drawPolygon(points, 3, &rect_dsc);

    // draw heading
    if (/*method != 1*/ true)
    {
        angle = heading - up; // rotate counterclockwise

        /*
        points[0].x = 0;
        points[0].y = COMPASS_RADIUS;
        points[1].x = 0 - COMPASS_RADIUS / 5;
        points[1].y = COMPASS_RADIUS / 2;
        points[2].x = COMPASS_RADIUS / 5;
        points[2].y = COMPASS_RADIUS / 2;

        RotateAndTranslate(points, base_x, base_y, 0 - angle);
        */
        points[0].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin(angle * DEG_TO_RAD));
        points[0].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos(angle * DEG_TO_RAD));
        points[1].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle + 18) * DEG_TO_RAD) / 2);
        points[1].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle + 18) * DEG_TO_RAD) / 2);
        points[2].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle - 18) * DEG_TO_RAD) / 2);
        points[2].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle - 18) * DEG_TO_RAD) / 2);

        rect_dsc.bg_opa = LV_OPA_60;
        rect_dsc.bg_color = lv_color_hex(0x0000FF);
        _ref->drawPolygon(points, 3, &rect_dsc);
    }

    // bearing
    if (bearing >= 0)
    {
        angle = bearing - up; // rotate counterclockwise

        /*
        points[0].x = 0;
        points[0].y = COMPASS_RADIUS;
        points[1].x = 0 - COMPASS_RADIUS / 5;
        points[1].y = COMPASS_RADIUS * 2;
        points[2].x = COMPASS_RADIUS / 5;
        points[2].y = COMPASS_RADIUS * 2;

        RotateAndTranslate(points, base_x, base_y, 0 - angle);
        */
        points[0].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin(angle * DEG_TO_RAD));
        points[0].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos(angle * DEG_TO_RAD));
        points[1].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle + 18) * DEG_TO_RAD) / 2);
        points[1].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle + 18) * DEG_TO_RAD) / 2);
        points[2].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle - 18) * DEG_TO_RAD) / 2);
        points[2].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle - 18) * DEG_TO_RAD) / 2);

        rect_dsc.bg_opa = LV_OPA_80;
        rect_dsc.bg_color = lv_color_hex(0x00FF00);
        rect_dsc.border_width = 1;
        rect_dsc.border_color = lv_color_hex(0x000000);
        _ref->drawPolygon(points, 3, &rect_dsc);
    }
}





////////////////////////////////////////////////////////////////////////////////////
// class VariometerWidget

VariometerWidget::VariometerWidget(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
{

}

void VariometerWidget::onCreate()
{

}

void VariometerWidget::update()
{
    // draw on reference-canvas
    if (updater)
        updater->onUpdate(this);
}

void VariometerWidget::draw(float vario)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_line_dsc_t line_dsc;
    lv_point_t points[3];

    lv_draw_rect_dsc_init(&rect_dsc);    
    lv_draw_line_dsc_init(&line_dsc);

    lv_coord_t x = 6 + _x;
    lv_coord_t y = 6 + _y;
    lv_coord_t w = _w - 12;
    lv_coord_t h = _h - 12;

    rect_dsc.outline_color = lv_color_hex(0x000000);
    rect_dsc.outline_width = 1;
    rect_dsc.outline_opa = LV_OPA_50;
    rect_dsc.bg_color = lv_color_hex(0x000000);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

    _ref->drawRect(x, y, w, h, &rect_dsc);

    line_dsc.color = lv_color_hex(0x808080);
    line_dsc.width = 1;
    points[0].x = x;
    points[0].y = y + h / 2;
    points[1].x = x + w;
    points[1].y = y + h / 2;
    _ref->drawLine(points, 2, &line_dsc);

    x += 4;
    y += 4;
    w -= 8;
    h -= 8;

    lv_coord_t h_half = h / 2;
    lv_coord_t h_mid = h / 2 + y;

    //vario = std::clamp<float>(vario, -5.0f, 5.0f);
    if (vario < -5.0f)
        vario = -5.0f;
    if (vario > 5.0f)
        vario = 5.0f;
    float v1, v2;

    if (vario > 0.0f)
    {
        v1 = vario;
        v2 = 0.0f;
    }
    else 
    {        
        v1 = 0.0f;
        v2 = vario;
    }

    lv_coord_t y1 = h_mid - h_half * v1 / 5.0f;
    lv_coord_t y2 = h_mid - h_half * v2 / 5.0f;

    rect_dsc.outline_width = 0;
    rect_dsc.bg_color = vario < 0.0f ? lv_color_hex(0x0000FF) : lv_color_hex(0xFF0000);
    rect_dsc.bg_opa = LV_OPA_COVER;

    _ref->drawRect(x, y1 < y2 ? y1 : y2, w, y2 - y1, &rect_dsc);    
}



////////////////////////////////////////////////////////////////////////////////////
// class ThermalAssistant

ThermalAssistant::ThermalAssistant(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
{
}

void ThermalAssistant::onCreate()
{
}

void ThermalAssistant::update()
{
    // draw on reference-canvas
    if (updater)
        updater->onUpdate(this);
}
