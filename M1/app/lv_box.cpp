// lv_box.cpp
//

#include <lvgl.h>
#include "logger.h"
#include "lv_box.h"


//
//
//

typedef struct _box_info
{
    lv_coord_t  magic;
    lv_obj_t*   title;
    lv_obj_t*   desc;

    lv_obj_t*   content;
} box_info_t;


//
//
//

lv_style_t* box_def_style = NULL;



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

        lv_style_set_pad_hor(box_def_style, 0);
        lv_style_set_pad_ver(box_def_style, 0);
        lv_style_set_border_width(box_def_style, 0);
        lv_style_set_border_side(box_def_style, 0);
        lv_style_set_radius(box_def_style, 0);

        lv_style_set_bg_color(box_def_style, lv_color_hex(0xFFFFFF));
    }
}

lv_obj_t* lv_box_create(lv_obj_t* parent, const char* title, const char* desc)
{
    lv_obj_t* box = lv_obj_create(parent);

    if (box_def_style)
        lv_obj_add_style(box, box_def_style, 0);

    box_info_t* info = (box_info_t *)lv_mem_alloc(sizeof(box_info_t));
    if (info)
    {
        info->magic = (lv_coord_t)0xDEADBEEF;
        info->title = NULL;
        info->desc = NULL;
        info->content = NULL;

        lv_obj_set_user_data(box, info);
        lv_box_set_title(box, title);
        lv_box_set_description(box, desc);
    }

    return box;
}

void lv_box_set_title(lv_obj_t* box, const char* title)
{
    box_info_t* info = (box_info_t *)lv_obj_get_user_data(box);

    if (info && info->magic == (lv_coord_t)0xDEADBEEF)
    {
        if (title && title[0])
        {
            if (!info->title)
                info->title = lv_label_create(box);

            if (info->title)
            {
                lv_label_set_text(info->title, title);
                lv_obj_set_style_text_font(info->title, &lv_font_montserrat_14, 0);
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
    if (info && info->magic == (lv_coord_t)0xDEADBEEF)
    {
        if (desc && desc[0])
        {
            if (!info->desc)
                info->desc = lv_label_create(box);

            if (info->desc)
            {
                lv_label_set_text(info->desc, desc);
                lv_obj_set_style_text_font(info->desc, &lv_font_montserrat_12, 0);
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

void lv_box_set_value_()
{
    lv_label_set_text_fmt(0, "%d", 3);
}


const char* lv_box_get_title(lv_box_type_t type)
{
    switch (type)
    {
    case ALTITUDE_GROUND:
        return "Altitude G.";
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
    case LIFT_vs_DRAG:
        return "L/D";
    case COMPASS:
        return NULL;
    case VSPEED_BAR:
        return NULL;
    case VSPEED_PROFILE:
        return NULL;
    case TRACK_FLIGHT:
        return NULL;
    default:
        return NULL;
    }
}

const char* lv_box_get_description(lv_box_type_t type)
{
    switch (type)
    {
    case ALTITUDE_GROUND:
        return "m";
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
    case LIFT_vs_DRAG:
        return NULL;
    case COMPASS:
        return NULL;
    case VSPEED_BAR:
        return NULL;
    case VSPEED_PROFILE:
        return NULL;
    case TRACK_FLIGHT:
        return NULL;
    default:
        return NULL;
    }
}
