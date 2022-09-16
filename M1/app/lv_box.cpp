// lv_box.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <lvgl.h>

#include "device_defines.h"
#include "logger.h"
#include "lv_box.h"
#include "lv_app.h"


//
//
//

typedef struct _box_info
{
    uint32_t        magic;
    lv_obj_t*       title;
    lv_obj_t*       desc;

    lv_box_type_t   box_type;
    lv_obj_t*       content;
} box_info_t;


typedef enum _box_content_type
{
	CT_INTEGER,
	CT_FLAOT,
    CT_DATETIME,
    CT_DATE,
    CT_TIME,
	CT_COMPASS,
    CT_PROFILE_ALTITUDE,
    CT_PROFILE_VSPEED,
    CT_VSPEED_BAR,

} box_content_type_t;

typedef struct _box_content
{
	box_content_type_t type;
	union 
    {
		int32_t*    piValue;    // CF_INTEGER
        float*      pfValue;    // CF_FLOAT, CT_VSPEED_BAR
        time_t*     ptValue;    // CF_DATE, CF_TIME, CF_DATETIME

        // compass
        struct {
            int32_t*    heading;
            int32_t*    bearing;
        } compass;

        // profile-altitude/vspeed
        struct {
            int32_t len;
            float*  data;
        } profile;
	};

} box_content_t;

//
//
//

lv_obj_t*           scrn_offline = NULL;
lv_obj_t*           boxes[BOX_COUNT];
lv_style_t*         box_def_style = NULL;

const lv_font_t*    font_title = &lv_font_montserrat_14;
const lv_font_t*    font_subtitle = &lv_font_montserrat_12;
const lv_font_t*    font_body = &lv_font_montserrat_48;


//
// compass
//

#define COMPASS_WIDTH       106
#define COMPASS_HEIGHT      106
#define COMPASS_RADIUS      53


static lv_color_t buf_compass[LV_CANVAS_BUF_SIZE_TRUE_COLOR(COMPASS_WIDTH, COMPASS_HEIGHT)];

// heading, bearing : angle(radian)
// method 
//  0 : up-side is north
//  1 : up-side is heading
//  2 : up-side is bearing

void RotateAndTranslate(lv_point_t* points, lv_coord_t org_x, lv_coord_t org_y, float angle)
{
    for (int i = 0; i < 3; i++)
    {
        float theta = angle * DEG_TO_RAD;
        lv_coord_t x = points[i].x * cos(theta) - points[i].y * sin(theta);
        lv_coord_t y = points[i].x * sin(theta) + points[i].y * cos(theta);
        
        points[i].x = org_x + x;
        points[i].y = org_y - y;
    }
}

void lv_compass_draw(lv_obj_t* canvas, lv_coord_t heading, lv_coord_t bearing, int32_t method)
{
    //
    lv_coord_t base_x = COMPASS_WIDTH / 2;
    lv_coord_t base_y = COMPASS_HEIGHT / 2;
    lv_coord_t angle, up = 0;
    
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
    lv_canvas_fill_bg(canvas, lv_color_hex(0xFFFFFF), LV_OPA_100);

    // draw circle
    arc_dsc.color = lv_color_hex(0x000000);
    arc_dsc.width = 1;
    arc_dsc.start_angle = 0;
    arc_dsc.end_angle = 360;
    arc_dsc.img_src = NULL;
    arc_dsc.opa = LV_OPA_40;
    arc_dsc.blend_mode = LV_BLEND_MODE_NORMAL;
    arc_dsc.rounded = 0;
    lv_canvas_draw_arc(canvas, base_x, base_y, COMPASS_RADIUS, 0, 360, &arc_dsc);

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
    lv_canvas_draw_polygon(canvas, points, 3, &rect_dsc);

    points[2].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin((angle + 140) * DEG_TO_RAD));
    points[2].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos((angle + 140) * DEG_TO_RAD));

    rect_dsc.bg_opa = LV_OPA_80;
    lv_canvas_draw_polygon(canvas, points, 3, &rect_dsc);

    // draw heading
    if (/*method != 1*/ true)
    {
        angle = heading - up; // rotate counterclockwise

        #if DRAW_LINE
        points[0].x = base_x;
        points[0].y = base_y;
        points[1].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin(angle * DEG_TO_RAD));
        points[1].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos(angle * DEG_TO_RAD));

        line_dsc.color = lv_color_hex(0x000000);
        line_dsc.width = 2;
        line_dsc.opa = LV_OPA_60;

        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
        #else // DRAW_TRIANGLE
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
        lv_canvas_draw_polygon(canvas, points, 3, &rect_dsc);
        #endif
    }

    // bearing
    if (bearing >= 0)
    {
        angle = bearing - up; // rotate counterclockwise

        #if DRAW_TRIANGLE
        points[0].x = base_x;
        points[0].y = base_y;
        points[1].x = base_x + (lv_coord_t)(COMPASS_RADIUS * sin(angle * DEG_TO_RAD));
        points[1].y = base_y - (lv_coord_t)(COMPASS_RADIUS * cos(angle * DEG_TO_RAD));

        line_dsc.color = lv_color_hex(0x0000FF);
        line_dsc.width = 2;
        line_dsc.dash_width = 6;
        line_dsc.dash_gap = 4;

        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
        #else
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
        lv_canvas_draw_polygon(canvas, points, 3, &rect_dsc);
        #endif
    }
}



//
//
//

void lv_box_init()
{
    if (!box_def_style)
    {
        box_def_style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        if (!box_def_style)
            return;

        lv_style_init(box_def_style);
        lv_style_set_pad_hor(box_def_style, 4);
        lv_style_set_pad_ver(box_def_style, 2);
        lv_style_set_radius(box_def_style, 0/*4*/);
        lv_style_set_border_width(box_def_style, 0/*1*/);
        lv_style_set_border_side(box_def_style, LV_BORDER_SIDE_NONE);

        lv_style_set_bg_color(box_def_style, lv_color_hex(0xFFFFFF));
    }

    if (!scrn_offline)
    {
        scrn_offline = lv_obj_create(NULL);
        if (!scrn_offline)
            return;

        for (int i = 0; i < BOX_COUNT; i++)
        {
            boxes[i] = lv_box_create(scrn_offline, lv_box_get_title((lv_box_type_t)i), lv_box_get_description((lv_box_type_t)i));
            if (boxes[i])
                lv_box_set_content(boxes[i], (lv_box_type_t)i);
        }
    }
}

lv_obj_t* lv_box_get_stock_object(lv_box_type_t type)
{
    if (type >= 0 && type < BOX_COUNT)
        return boxes[type];
    
    return NULL;
}

void lv_box_detach_all()
{
    for (int i = 0; i < BOX_COUNT; i++)
        lv_obj_set_parent(boxes[i], scrn_offline);
}

lv_obj_t* lv_box_create(lv_obj_t* parent, const char* title, const char* desc)
{
    lv_obj_t* box = lv_obj_create(parent);

    if (box_def_style)
        lv_obj_add_style(box, box_def_style, 0);

    box_info_t* info = (box_info_t *)lv_mem_alloc(sizeof(box_info_t));
    if (info)
    {
        info->magic = 0xDEADBEEF;
        info->title = NULL;
        info->desc = NULL;
        info->content = NULL;

        lv_obj_set_user_data(box, info);
        lv_box_set_title(box, title);
        lv_box_set_description(box, desc);
    }

    return box;
}

lv_obj_t* lv_box_create_canvas(lv_obj_t* box, void* buf, int32_t w, int32_t h)
{
    lv_obj_t* canvas = lv_canvas_create(box);
    if (!canvas)
        return NULL;

    lv_obj_set_align(canvas, LV_ALIGN_CENTER);
    lv_canvas_set_buffer(canvas, buf, w, h, LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(canvas, lv_color_hex(0xFFFFFF), LV_OPA_100);

    return canvas;
}


void lv_box_set_title(lv_obj_t* box, const char* title)
{
    box_info_t* info = (box_info_t *)lv_obj_get_user_data(box);

    if (info && info->magic == 0xDEADBEEF)
    {
        if (title && title[0])
        {
            if (!info->title)
                info->title = lv_label_create(box);

            if (info->title)
            {
                lv_label_set_text(info->title, title);
                lv_obj_set_style_text_font(info->title, font_title, 0);
                lv_obj_align(info->title, LV_ALIGN_TOP_LEFT, 0, 0);
            }
        }
        else
        {
            if (info->title)
            {
                lv_obj_del(info->title);
                info->title = NULL;
            }
        }
    }
}

void lv_box_set_description(lv_obj_t* box, const char* desc)
{
    box_info_t* info = (box_info_t *)lv_obj_get_user_data(box);
    if (info && info->magic == 0xDEADBEEF)
    {
        if (desc && desc[0])
        {
            if (!info->desc)
                info->desc = lv_label_create(box);

            if (info->desc)
            {
                lv_label_set_text(info->desc, desc);
                lv_obj_set_style_text_font(info->desc, font_subtitle, 0);
                lv_obj_align(info->desc, LV_ALIGN_TOP_RIGHT, 0, 0);
            }
        }
        else
        {
            if (info->desc)
            {
                lv_obj_del(info->desc);
                info->desc = NULL;
            }

        }
    }
}

extern void* lv_box_get_content_data(lv_box_type_t type)
{
    return NULL;
}

void lv_box_set_content(lv_obj_t* box, lv_box_type_t type)
{
    box_info_t* info = (box_info_t *)lv_obj_get_user_data(box);
    if (!info || info->magic != 0xDEADBEEF)
        return;

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
    case SENSOR_PRESSURE:
    case SENSOR_TEMPERATURE:
    case SENSOR_HUMIDITY:
        info->box_type = type;
        info->content = lv_label_create(box);
        lv_obj_align(info->content, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        lv_obj_set_style_text_font(info->content, font_body, 0);
        lv_label_set_text(info->content, "");
        break;
    case COMPASS:
        info->box_type = type;
        info->content = lv_box_create_canvas(box, buf_compass, COMPASS_WIDTH, COMPASS_HEIGHT);
        lv_compass_draw(info->content, 0, 0, 0);
        break;
    case VSPEED_BAR:
    case VSPEED_PROFILE:
    case TRACK_FLIGHT:
        break;
    } 
}


const char* lv_box_get_title(lv_box_type_t type)
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

const char* lv_box_get_description(lv_box_type_t type)
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

void lv_box_set_content_text(lv_obj_t* label, const char* format, ...)
{
    static char buf[32];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    lv_label_set_text(label, buf);
}

void lv_box_update(lv_obj_t* box)
{
    box_info_t* info = (box_info_t *)lv_obj_get_user_data(box);
    if (!info || info->magic != 0xDEADBEEF)
        return;

    app_conf_t* conf = app_get_conf();

    switch (info->box_type)
    {
    case ALTITUDE_GROUND:
        lv_box_set_content_text(info->content, "%.0f", conf->altitudeGPS);
        break;
    case ALTITUDE_BARO:
        lv_box_set_content_text(info->content, "%.0f", conf->altitudeBaro);
        break;
    case ALTITUDE_AGL:
        lv_box_set_content_text(info->content, "%.0f", conf->altitudeAGL);
        break;
    case ALTITUDE_PROFILE:
        break;
    case SPEED_GROUND:
        lv_box_set_content_text(info->content, "%.0f", conf->speedGround);
        break;
    case SPEED_AIR:
        break;
    case SPEED_VERTICAL:
        lv_box_set_content_text(info->content, "%.1f", conf->speedVertActive);
        break;
    case SPEED_VERTICAL_LAZY:
        lv_box_set_content_text(info->content, "%.1f", conf->speedVertLazy);
        break;
    case TRACK_HEADING:
        lv_box_set_content_text(info->content, "%d", conf->heading);
        break;
    case TARCK_BEARING:
        lv_box_set_content_text(info->content, "%d", conf->bearing);
        break;
    case TIME_FLIGHT:
        if (conf->timeFly > 0)
        {
            time_t temp = conf->timeFly % 3600;
            time_t h = conf->timeFly / 3600;
            time_t m = temp / 60;
            time_t s = temp % 60;
            if (h != 0)
                lv_box_set_content_text(info->content, "%d:%02d:%02d", h, m, s);
            else
                lv_box_set_content_text(info->content, "%02d:%02d", m, s);
        }
        else
        {
            lv_box_set_content_text(info->content, "");
        }
        break;
    case TIME_CURRENT:
        break;
    case TIME_TO_NEXT_WAYPOINT:
        break;
    case TIME_REMAIN:
        break;
    case DISTANCE_TAKEOFF:
        break;
    case DISTANCE_LANDING:
        break;
    case DISTANCE_NEXT_WAYPOINT:
        break;
    case DISTANCE_FLIGHT:
        break;
    case GLIDE_RATIO:
        lv_box_set_content_text(info->content, "%.1f", conf->glideRatio);
        break;
    case COMPASS:
        lv_compass_draw(info->content, conf->heading, conf->bearing, 0);
        break;
    case VSPEED_BAR:
        break;
    case VSPEED_PROFILE:
        break;
    case TRACK_FLIGHT:
        break;
    case SENSOR_PRESSURE:
        lv_box_set_content_text(info->content, "%.0f", conf->pressure);
        break;
    case SENSOR_TEMPERATURE:
        lv_box_set_content_text(info->content, "%.1f", conf->temperature);
        break;
    case SENSOR_HUMIDITY:
        break;
    }        
}