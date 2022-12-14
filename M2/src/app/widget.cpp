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


Widget::Widget() : updateCb(NULL)
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
    if (updateCb)
        updateCb->onUpdate(this);
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
    if (updateCb)
        updateCb->onUpdate(this);
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
    if (updateCb)
        updateCb->onUpdate(this);
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
    if (updateCb)
        updateCb->onUpdate(this);
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
    if (updateCb)
        updateCb->onUpdate(this);
}

void VariometerWidget::draw(float vario)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_line_dsc_t line_dsc;
    lv_point_t points[2];

    lv_draw_rect_dsc_init(&rect_dsc);
    lv_draw_line_dsc_init(&line_dsc);

    lv_coord_t x = _x;
    lv_coord_t w = _w;

#define BAR_COUNT       (5)
#define BAR_SPACE       (2)
#define BAR_SIDE_EDGE   (4)

    lv_coord_t bar_space = 2;
    lv_coord_t bar_h = ((_h / 2) - (bar_space * BAR_COUNT + 1)) / BAR_COUNT;
    lv_coord_t bar_w = _w - BAR_SIDE_EDGE * 2;
    lv_coord_t draw_x = _x;
    lv_coord_t draw_w = _w;
    lv_coord_t draw_center = _y + _h / 2;
    lv_coord_t draw_half = (bar_h * BAR_COUNT + bar_space * (BAR_COUNT + 1));
    lv_coord_t draw_y = draw_center - draw_half;
    lv_coord_t draw_h = draw_half * 2 + 1;

    // draw border
    rect_dsc.outline_color = lv_color_hex(0x808080);
    rect_dsc.outline_width = 1;
    rect_dsc.outline_opa = LV_OPA_COVER;
    rect_dsc.bg_color = lv_color_hex(0x000000);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

    _ref->drawRect(draw_x, draw_y, draw_w, draw_h, &rect_dsc);

    line_dsc.color = lv_color_hex(0x808080);
    line_dsc.width = 1;
    points[0].x = draw_x;
    points[0].y = draw_center;
    points[1].x = draw_x + draw_w;
    points[1].y = draw_center;
    _ref->drawLine(points, 2, &line_dsc);

    // draw bar
    int bar_index = (int)floor(vario + 0.8); // ..., -1: [-1.8, -0.8), 0: [-0.8, 0.2), 1: [0.2, 1.2), ...

    if (bar_index >= BAR_COUNT * 2)
        bar_index = BAR_COUNT * 2 - 1;
    if (bar_index <= BAR_COUNT * -2)
        bar_index = BAR_COUNT * -2 + 1;

    rect_dsc.outline_width = 1;
    rect_dsc.outline_color = lv_color_hex(bar_index > 0 ? 0xFF8080 : 0x8080FF);
    rect_dsc.bg_color = lv_color_hex(bar_index > 0 ? 0xFF0000 : 0x0000FF);
    rect_dsc.bg_opa = LV_OPA_COVER;

    if (bar_index > 0)
    {
        // BAR_COUNT = 5
        // bar_index   = [1, 2, 3, 4, 5, 6, 7, 8, 9]
        // bar_s_index = [0, 0, 0, 0, 0, 1, 2, 3, 4]
        // bar_e_index = [1, 2, 3, 4, 5, 5, 5, 5, 5]
        int s = 0, e = BAR_COUNT;

        if (bar_index > BAR_COUNT)
            s = bar_index % BAR_COUNT;
        if (bar_index <= BAR_COUNT)
            e = (bar_index - 1) % BAR_COUNT + 1;
        
        for (int i = s; i < e; i++)
        {
            lv_coord_t x = draw_x + BAR_SIDE_EDGE;
            lv_coord_t y = draw_center - (bar_h + BAR_SPACE) * (i + 1);

            _ref->drawRect(x, y, bar_w, bar_h, &rect_dsc);    
        }
    }
    else if (bar_index < 0)
    {
        // BAR_COUNT = 5
        // bar_index   = [-1, -2, -3, -4, -5, -6, -7, -8, -9]
        // bar_s_index = [ 0,  0,  0,  0,  0,  1,  2,  3,  4]
        // bar_e_index = [ 1,  2,  3,  4,  5,  5,  5,  5,  5]
        int s = 0, e = BAR_COUNT;

        bar_index = 0 - bar_index;
        if (bar_index > BAR_COUNT)
            s = bar_index % BAR_COUNT;
        if (bar_index <= BAR_COUNT)
            e = (bar_index - 1) % BAR_COUNT + 1;
        
        for (int i = s; i < e; i++)
        {
            lv_coord_t x = draw_x + BAR_SIDE_EDGE;
            lv_coord_t y = draw_center + BAR_SPACE + (bar_h + BAR_SPACE) * i;

            _ref->drawRect(x, y, bar_w, bar_h, &rect_dsc);    
        }
    } 
}



////////////////////////////////////////////////////////////////////////////////////
// class ThermalAssistant

static void init_line_dsc(float vario, lv_draw_line_dsc_t* line_dsc)
{
    if (vario < -1.0)
    {
        line_dsc->width = 4;
        line_dsc->color = lv_color_hex(0x0000FF);
    }
    else if (vario < 0.2)
    {
        line_dsc->width = 2;
        line_dsc->color = lv_color_hex(0x808080);
    }
    else if (vario < 1.0)
    {
        line_dsc->width = 3;
        line_dsc->color = lv_color_hex(0xFF0000);
    }
    else
    {
        line_dsc->width = 5;
        line_dsc->color = lv_color_hex(0xFFFF00);
    }
}

ThermalAssistant::ThermalAssistant(CanvasWidget* ref)
    : CanvasVirtualWidget(ref)
    , up(0)
    , heading(-1)
    , bearing(-1)
    , wind(-1)
    , trackCount(0)
{
}

void ThermalAssistant::update()
{
    // draw on reference-canvas
    if (updateCb)
        updateCb->onUpdate(this);
}

void ThermalAssistant::postUpdate()
{
    #if 0
    drawTrack();
    drawWindDirection();
    drawFlight(heading, bearing, up);
    #endif
}

void ThermalAssistant::setOption(int16_t up, int16_t heading, int16_t bearing, int16_t wind)
{
    this->up = up;
    this->heading = heading;
    this->bearing = bearing;
    this->wind = wind;
}

#define RADIUS 					(6371000) // 6371e3

#define TO_RADIAN(x)			((x)*(PI/180))
#define DISTANCE_SIMPLE(angle)	(2.0 * RADIUS * sin(TO_RADIAN((angle) / 2)))

void ThermalAssistant::setTrack(TrackHistory* tracks, int16_t front, int16_t rear)
{
    int index = rear, count = 0;

    if (false && rear != front) // empty?
    {
        int latest = (front + (MAX_TRACK_HISTORY - 1)) & (MAX_TRACK_HISTORY - 1);
        TrackHistory* last = tracks + latest;

        while (index != latest)
        {
            TrackHistory* track = tracks + index;

            trackDistance[count].dx = -DISTANCE_SIMPLE(last->lon - track->lon);
            trackDistance[count].dy = DISTANCE_SIMPLE(last->lat - track->lat);
            trackVario[count] = track->vario;

            index = (index + 1) & (MAX_TRACK_HISTORY - 1);
            count = count + 1;
        }
    }

    trackCount = count;
    //LOGv("History count = %d", trackCount);
    //for (int i = trackCount - 1; i >= 0 && i > trackCount - 4; --i)
    //    LOGv(" %d:  (%f, %f) %f", i + 1, trackDistance[i].dx, trackDistance[i].dy, trackVario[i]);
    //for (int i = 2; i >= 0 && i < trackCount; --i)
    //    LOGv(" %d:  (%f, %f) %f", i + 1, trackDistance[i].dx, trackDistance[i].dy, trackVario[i]);
}

void ThermalAssistant::drawTrack()
{
#define ZOOM_FACTOR			(1.0)

    // 
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_line_dsc_t line_dsc;
    lv_point_t points[2];

    lv_draw_rect_dsc_init(&rect_dsc);
    lv_draw_line_dsc_init(&line_dsc);

    rect_dsc.outline_color = lv_color_hex(0xFFFFFF);
    rect_dsc.outline_width = 0;
    rect_dsc.outline_opa = LV_OPA_COVER;
    rect_dsc.bg_color = lv_color_hex(0xFFFFFF);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

    line_dsc.color = lv_color_hex(0x808080);
    line_dsc.width = 1;

    // draw border & background
    _ref->drawRect(_x, _y, _w, _h, &rect_dsc);

	float theta = TO_RADIAN(180 - heading);

    for (int i = 0; i < trackCount; i++)
    {
        float x0 = trackDistance[i].dx * ZOOM_FACTOR;
        float y0 = trackDistance[i].dy * ZOOM_FACTOR;

		points[0].x = (_x + _w / 2) - (lv_coord_t)(x0 * cos(theta) - y0 * sin(theta));
		points[0].y = (_y + _h / 2) - (lv_coord_t)(x0 * sin(theta) + y0 * cos(theta));

        if (i > 0)
        {
            //if (trackVario[i] < 0)
            //    line_dsc.width = 1;
            //else if (trackVario[i] < 1.0f)
            //    line_dsc.width = 2;
            //else
            //    line_dsc.width = 3;

            init_line_dsc(trackVario[i], &line_dsc);

            _ref->drawLine(points, 2, &line_dsc);
        }

        points[1].x = points[0].x;
        points[1].y = points[0].y;
    }
}

void ThermalAssistant::drawTrack(FlightState& state, float up)
{
#define ZOOM_FACTOR			(1.0)
    int16_t size = (state.frontPoint - state.rearPoint) & (MAX_TRACK_HISTORY - 1);
    if (size < 2)
        return;

    // 
    lv_draw_rect_dsc_t rect_dsc;
    //lv_draw_arc_dsc_t arc_dsc;
    lv_draw_line_dsc_t line_dsc;
    lv_point_t points[2];

    lv_draw_rect_dsc_init(&rect_dsc);
    //lv_draw_arc_dsc_init(&arc_dsc);
    lv_draw_line_dsc_init(&line_dsc);

    rect_dsc.outline_color = lv_color_hex(0xFFFFFF);
    rect_dsc.outline_width = 0;
    rect_dsc.outline_opa = LV_OPA_COVER;
    rect_dsc.bg_color = lv_color_hex(0xFFFFFF);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

    //arc_dsc.color = lv_color_hex(0x000000);
    //arc_dsc.width = 1;
    //arc_dsc.start_angle = 0;
    //arc_dsc.end_angle = 360;
    //arc_dsc.opa = LV_OPA_COVER;

    //line_dsc.color = lv_color_hex(0x808080);
    //line_dsc.width = 1;

    // draw border & background
    _ref->drawRect(_x, _y, _w, _h, &rect_dsc);

    #if 0
	// draw track
    //rect_dsc.outline_color = lv_color_hex(0x000000);
    //rect_dsc.bg_opa = LV_OPA_TRANSP;

	float theta = TO_RADIAN(180 - up);

    for (int i = state.rearPoint; i != state.frontPoint; )
    {
        float x0 = state.trackDistance[i].dx * ZOOM_FACTOR;
        float y0 = state.trackDistance[i].dy * ZOOM_FACTOR;

		points[0].x = (_x + _w / 2) - (lv_coord_t)(x0 * cos(theta) - y0 * sin(theta));
		points[0].y = (_y + _h / 2) - (lv_coord_t)(x0 * sin(theta) + y0 * cos(theta));

        if (i != state.rearPoint)
        {
            if (state.trackPoints[i].vario < 0)
                line_dsc.width = 1;
            else if (state.trackPoints[i].vario < 1.0f)
                line_dsc.width = 2;
            else
                line_dsc.width = 3;

            _ref->drawLine(points, 2, &line_dsc);
        }

        points[1].x = points[0].x;
        points[1].y = points[0].y;

        i = (i + 1) % MAX_TRACK_HISTORY;
    }
    #else
    int16_t from = state.rearPoint;
    int16_t to = (state.frontPoint + MAX_TRACK_HISTORY - 1) & (MAX_TRACK_HISTORY - 1);

    float dist = GET_DISTANCE(state.trackHistory[to].lat, state.trackHistory[to].lon, state.trackHistory[from].lat, state.trackHistory[from].lon);
    int16_t bearing = GET_BEARING(state.trackHistory[to].lat, state.trackHistory[to].lon, state.trackHistory[from].lat, state.trackHistory[from].lon);
    LOGd("dist = %f, bearing = %d, size = %d", dist, bearing, size);

	if (bearing > 180)
		bearing = bearing - 360;
	if (bearing < -180)
		bearing = bearing + 360;

    lv_coord_t cx = _x + _w / 2;
    lv_coord_t cy = _y + _h / 2;

	float dx = dist * ZOOM_FACTOR * std::sin(TO_RADIAN(bearing - up));
	float dy = dist * ZOOM_FACTOR * std::cos(TO_RADIAN(bearing - up));
	float x = cx + dx;
	float y = cy - dy;
    points[0].x = (lv_coord_t)x;
    points[0].y = (lv_coord_t)y;

    for (int16_t i = from; i != to; )
    {
		double track = state.trackHistory[i].track;

		if (track > 180)
			track = track - 360;
		if (track < -180)
			track = track + 360;

		float dx = state.trackHistory[i].dist * ZOOM_FACTOR * std::sin(TO_RADIAN(track - up));
		float dy = state.trackHistory[i].dist * ZOOM_FACTOR * std::cos(TO_RADIAN(track - up));
		x += dx;
		y -= dy;

        points[1].x = (lv_coord_t)x;
        points[1].y = (lv_coord_t)y;

        //if (state.trackPoints[i].vario < 0)
        //    line_dsc.width = 1;
        //else if (state.trackPoints[i].vario < 1.0f)
        //    line_dsc.width = 2;
        //else
        //    line_dsc.width = 3;

        init_line_dsc(state.trackPoints[i].vario, &line_dsc);

        _ref->drawLine(points, 2, &line_dsc);        

        points[0].x = points[1].x;
        points[0].y = points[1].y;

        i = (i + 1) & (MAX_TRACK_HISTORY - 1);
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
