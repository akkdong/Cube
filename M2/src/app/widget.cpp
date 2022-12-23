// widget.cpp
//

#include <stdint.h>
#include <math.h>
#include <algorithm>

#include "board.h"
#include "logger.h"
#include "utils.h"
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

lv_obj_t* Widget::createObject(lv_obj_t* parent)
{
    //LOGv("Widget::createObject()");
    lv_obj_t* obj = DisplayObject::createObject(parent);
    if (!obj)
        return NULL;

    if (style_default)
        lv_obj_add_style(obj, style_default, 0);
    
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    return obj;
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

lv_obj_t* Annunciator::createObject(lv_obj_t* parent)
{
    //LOGv("Annunciator::createObject()");
    lv_obj_t* ann = Widget::createObject(parent);
    if (!ann)
        return NULL;

    lv_obj_set_style_pad_hor(ann, 4, 0);
    lv_obj_set_style_pad_ver(ann, 0, 0);
    lv_obj_set_style_radius(ann, 0, 0);
    lv_obj_set_style_border_width(ann, 2, 0);
    lv_obj_set_style_border_color(ann, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_color(ann, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_opa(ann, LV_OPA_100, 0);

    // create sub-objects
    status = lv_label_create(ann);
    if (status)
    {
        lv_obj_set_style_text_font(status, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(status, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(status, LV_ALIGN_LEFT_MID, 0, 0);
        lv_label_set_text(status, "");
    }

    clock = lv_label_create(ann);
    if (clock)
    {
        lv_obj_set_style_text_font(clock, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(clock, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(clock, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_label_set_text(clock, "00:00:00");
    }

    return ann;
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

const lv_font_t* NumberBox::font_title = &lv_font_montserrat_24;
const lv_font_t* NumberBox::font_subtitle = &lv_font_montserrat_18;
const lv_font_t* NumberBox::font_body = &font_en_bold_64;


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
        return "Speed Vz.";
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
// class ProfileWidget

ProfileWidget::ProfileWidget() : dataType(UNDEFINED)
{
}

//
void ProfileWidget::update()
{
    if (updater)
        updater->onUpdate(this);
}

lv_obj_t* ProfileWidget::createObject(lv_obj_t* parent)
{
    return Widget::createObject(parent);
}



////////////////////////////////////////////////////////////////////////////////////
// class CanvasWidget

CanvasWidget::CanvasWidget(int width, int height) 
    : _w(width)
    , _h(height)
    , _buffer(NULL)
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

lv_obj_t* CanvasWidget::createObject(lv_obj_t* parent)
{
    #if 0
    lv_obj_t* obj = Widget::createObject(parent);
    if (!obj)
        return NULL;

    lv_obj_set_style_pad_hor(obj, 0, 0);
    lv_obj_set_style_pad_ver(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);

    //
    _canvas = lv_canvas_create(obj);
    if (_canvas)
    {
        lv_obj_set_align(_canvas, LV_ALIGN_CENTER);
        lv_canvas_set_buffer(_canvas, _buffer, _w, _h, LV_IMG_CF_TRUE_COLOR_ALPHA);
        lv_canvas_fill_bg(_canvas, lv_color_hex(0xFFFFFF), LV_OPA_TRANSP);
    }

    return obj;
    #else
    lv_obj_t* obj = lv_canvas_create(parent);
    if (!obj)
        return NULL;

    lv_obj_set_style_pad_hor(obj, 0, 0);
    lv_obj_set_style_pad_ver(obj, 0, 0);

    lv_canvas_set_buffer(obj, _buffer, _w, _h, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_canvas_fill_bg(obj, lv_color_hex(0xFFFFFF), LV_OPA_TRANSP);
    #endif

    return obj;
}

void CanvasWidget::update()
{
    // erase backgournd
    lv_canvas_fill_bg(_this, lv_color_hex(0xFFFFFF), LV_OPA_TRANSP);

    // each canvas-virtual-object will draw on this canvas
    // ...
}

void CanvasWidget::drawRect(lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, const lv_draw_rect_dsc_t * draw_dsc)
{
    lv_canvas_draw_rect(_this, x, y, w, h, draw_dsc);
}

void CanvasWidget::drawText(lv_coord_t x, lv_coord_t y, lv_coord_t max_w, lv_draw_label_dsc_t * draw_dsc, const char * txt)
{
    lv_canvas_draw_text(_this, x, y, max_w, draw_dsc, txt);
}

void CanvasWidget::drawImage(lv_coord_t x, lv_coord_t y, const void * src, const lv_draw_img_dsc_t * draw_dsc)
{
    lv_canvas_draw_img(_this, x, y, src, draw_dsc);
}

void CanvasWidget::drawLine(const lv_point_t points[], uint32_t point_cnt, const lv_draw_line_dsc_t * draw_dsc)
{
    lv_canvas_draw_line(_this, points, point_cnt, draw_dsc);
}

void CanvasWidget::drawArc(lv_coord_t x, lv_coord_t y, lv_coord_t r, int32_t start_angle, int32_t end_angle, const lv_draw_arc_dsc_t * draw_dsc)
{
    lv_canvas_draw_arc(_this, x, y, r, start_angle, end_angle, draw_dsc);
}

void CanvasWidget::drawPolygon(const lv_point_t points[], uint32_t point_cnt, const lv_draw_rect_dsc_t * draw_dsc)
{
    lv_canvas_draw_polygon(_this, points, point_cnt, draw_dsc);
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
    if (!_ref || !_ref->getObject())
        return false;

    //if (lv_obj_get_parent(_ref->getObject()) != parent->getObject())
    //  return false;

    // skip object-creation, just call onCreate only
    #if 1
    lv_async_call(_onCreate, this);
    #else
    this->onCreate();
    #endif

    return true;
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

extern const lv_img_dsc_t letter_n_16;


CompassWidget::CompassWidget(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
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
    // 1. up: north (method: 0)
    //  1-1: no bearing
    //    north: N letter
    //    heading: red arrow
    //  1-2: bearing
    //    north: N letter
    //    heading: red arrow
    //    bearing: blue arrow
    // 2. up: heading (method: 1)
    //  2-1: no bearing
    //    north: red arrow
    //    heading: N/A
    //  2-2: bearing
    //    north: red arrow
    //    heading: N/A
    //    bearing: blue arrow
    // 3. up: bearing (method: 2)
    //    north: N letter
    //    heading: red arrow
    //    bearing: blue arrow
    //

    lv_coord_t base_x = this->_w / 2 + _x;
    lv_coord_t base_y = this->_h / 2 + _y;
    lv_coord_t angle, up = 0;
    lv_coord_t radius = this->_w / 2 - 6;
    lv_point_t points;

    if (method == 2 && bearing < 0) // bearing < 0 : n/a
        method = 1;

    if (method == 1)
        up = heading;
    else if (method == 2) 
        up = bearing;
    // else up = 0 

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_img_dsc_t image_dsc;

    lv_draw_arc_dsc_init(&arc_dsc);
    lv_draw_rect_dsc_init(&rect_dsc);
    lv_draw_img_dsc_init(&image_dsc);

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

    _ref->drawArc(base_x, base_y, radius + 2, 0, 360, &arc_dsc);

    // draw north-letter
    angle = 0 - up; // real-north = rotate counterclockwise

    image_dsc.recolor = lv_color_hex(0xFF0000);
    image_dsc.recolor_opa = LV_OPA_COVER;
    //image_dsc.zoom = 256;
    //image_dsc.angle = 450;

    points.x = base_x + (lv_coord_t)((radius - 10) * sin(angle * DEG_TO_RAD));
    points.y = base_y - (lv_coord_t)((radius - 10) * cos(angle * DEG_TO_RAD));

    int offsetx = letter_n_16.header.w / 2;
    int offsety = letter_n_16.header.h / 2;

    _ref->drawImage(points.x - offsetx, points.y - offsety, &letter_n_16, &image_dsc);

    // draw heading
    angle = heading - up; // rotate counterclockwise
    drawArrow(base_x, base_y, radius, angle, lv_color_hex(0x303030));

    // draw bearing
    if (bearing >= 0)
    {
        angle = bearing - up; // rotate counterclockwise
        drawArrow(base_x, base_y, radius - 5, angle, lv_color_hex(0xF01010));
    }
}

void CompassWidget::drawArrow(lv_coord_t cx, lv_coord_t cy, lv_coord_t radius, lv_coord_t angle, lv_color_t color)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_point_t points[4];

    points[0].x = cx + (lv_coord_t)(radius * sin(angle * DEG_TO_RAD));
    points[0].y = cy - (lv_coord_t)(radius * cos(angle * DEG_TO_RAD));
    points[1].x = cx + (lv_coord_t)(radius / 3 * sin((angle + 180) * DEG_TO_RAD));
    points[1].y = cy - (lv_coord_t)(radius / 3 * cos((angle + 180) * DEG_TO_RAD));
    points[2].x = cx + (lv_coord_t)(radius * sin((angle - 140) * DEG_TO_RAD));
    points[2].y = cy - (lv_coord_t)(radius * cos((angle - 140) * DEG_TO_RAD));

    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = color;
    rect_dsc.bg_opa = LV_OPA_COVER;
    _ref->drawPolygon(points, 3, &rect_dsc);

    points[2].x = cx + (lv_coord_t)(radius * sin((angle + 140) * DEG_TO_RAD));
    points[2].y = cy - (lv_coord_t)(radius * cos((angle + 140) * DEG_TO_RAD));

    rect_dsc.bg_color = lv_color_lighten(color, LV_OPA_30);
    rect_dsc.bg_opa = LV_OPA_COVER;
    _ref->drawPolygon(points, 3, &rect_dsc);  
}



////////////////////////////////////////////////////////////////////////////////////
// class VariometerWidget

VariometerWidget::VariometerWidget(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
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
    rect_dsc.bg_color = lv_color_black(); // vario < 0.0f ? lv_color_hex(0x0000FF) : lv_color_hex(0xFF0000);
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    rect_dsc.bg_grad.stops_count = 2;
    rect_dsc.bg_grad.stops[0].color = vario < 0.0f ? lv_color_hex(0x000040) : lv_color_hex(0xFF0000);
    rect_dsc.bg_grad.stops[1].color = vario < 0.0f ? lv_color_hex(0x0000FF) : lv_color_hex(0x400000);

    _ref->drawRect(x, y1 < y2 ? y1 : y2, w, y2 - y1, &rect_dsc);    
}



////////////////////////////////////////////////////////////////////////////////////
// class ThermalAssistant

ThermalAssistant::ThermalAssistant(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
{
}

void ThermalAssistant::update()
{
    // draw on reference-canvas
    if (updater)
        updater->onUpdate(this);
}

void ThermalAssistant::drawTrack(FlightState& state, float heading)
{
#define ZOOM_FACTOR			(0.8)

    // 
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_arc_dsc_t arc_dsc;

    lv_draw_rect_dsc_init(&rect_dsc);
    lv_draw_arc_dsc_init(&arc_dsc);

    rect_dsc.outline_color = lv_color_hex(0xFFFFFF);
    rect_dsc.outline_width = 0;
    rect_dsc.outline_opa = LV_OPA_COVER;
    rect_dsc.bg_color = lv_color_hex(0xFFFFFF);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

    arc_dsc.color = lv_color_hex(0x000000);
    arc_dsc.width = 1;
    arc_dsc.start_angle = 0;
    arc_dsc.end_angle = 360;
    arc_dsc.opa = LV_OPA_COVER;    


    // draw border & background
    _ref->drawRect(_x, _y, _w, _h, &rect_dsc);


	// draw track
    rect_dsc.outline_color = lv_color_hex(0x000000);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

	float theta = TO_RADIAN(180 - heading);

	for (int i = state.rearPoint; i != state.frontPoint; )
	{
#if 1 // 0 clock is heading
		float x0 = state.trackDistance[i].dx * ZOOM_FACTOR;
		float y0 = state.trackDistance[i].dy * ZOOM_FACTOR;

		int16_t x = (_x + _w / 2) - (int16_t)(x0 * cos(theta) - y0 * sin(theta));
		int16_t y = (_y + _h / 2) - (int16_t)(x0 * sin(theta) + y0 * cos(theta));
#else // 0 clock is north
		int16_t x = _x + _w / 2;
		int16_t y = _y + _h / 2;

		x -= state.trackDistance[i].dx * ZOOM_FACTOR;
		y += state.trackDistance[i].dy * ZOOM_FACTOR;
#endif

		if (_x + 2 < x && _y + 2 < y && x < _x + _w - 2 && y < _y + _h - 2)
		{
			if (state.trackPoints[i].vario < 0)
			{
				_ref->drawRect(x - 4, y - 4, 8, 8, &rect_dsc);
			}
			else
			{
				int16_t r = 2;

				if (state.trackPoints[i].vario > 1)
					r = (state.trackPoints[i].vario > 2) ? 8 : 5;

                _ref->drawArc(x, y, r, 0, 360, &arc_dsc);
			}
		}

		i = (i + 1) % MAX_TRACK_HISTORY;
	}

	// draw glider
    #if 0
	{
		int16_t cx = _x + _w / 2;
		int16_t cy = _y + _h / 2;

		drawLine(cx - 10, cy, cx, cy - 18, COLOR_BLACK);
		drawLine(cx, cy - 18, cx + 10, cy, COLOR_BLACK);
	}
    #endif
}

void ThermalAssistant::drawCompass()
{
}

void ThermalAssistant::drawWindDirection()
{
}

void ThermalAssistant::drawFlight(float heading, float bearing, int method)
{
    lv_coord_t base_x = this->_w / 2 + _x;
    lv_coord_t base_y = this->_h / 2 + _y;    
    lv_coord_t angle, up = 0;
    lv_coord_t radius = 24;

    if (method == 2 && bearing < 0) // bearing < 0 : n/a
        method = 1;

    if (method == 1)
        up = heading;
    else if (method == 2) 
        up = bearing;
    // else up = 0   

    angle = heading - up; // rotate counterclockwise

    // 
    lv_draw_rect_dsc_t rect_dsc;
    lv_point_t points[4];
    lv_color_t color = lv_color_hex(0x303030);

    points[0].x = base_x + (lv_coord_t)(radius * sin(angle * DEG_TO_RAD));
    points[0].y = base_y - (lv_coord_t)(radius * cos(angle * DEG_TO_RAD));
    points[1].x = base_x + (lv_coord_t)(radius / 3 * sin((angle + 180) * DEG_TO_RAD));
    points[1].y = base_y - (lv_coord_t)(radius / 3 * cos((angle + 180) * DEG_TO_RAD));
    points[2].x = base_x + (lv_coord_t)(radius * sin((angle - 140) * DEG_TO_RAD));
    points[2].y = base_y - (lv_coord_t)(radius * cos((angle - 140) * DEG_TO_RAD));

    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = color;
    rect_dsc.bg_opa = LV_OPA_COVER;
    _ref->drawPolygon(points, 3, &rect_dsc);

    points[2].x = base_x + (lv_coord_t)(radius * sin((angle + 140) * DEG_TO_RAD));
    points[2].y = base_y - (lv_coord_t)(radius * cos((angle + 140) * DEG_TO_RAD));

    rect_dsc.bg_color = lv_color_lighten(color, LV_OPA_30);
    rect_dsc.bg_opa = LV_OPA_COVER;
    _ref->drawPolygon(points, 3, &rect_dsc);
}
