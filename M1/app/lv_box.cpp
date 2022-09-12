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
        lv_style_set_pad_hor(box_def_style, 0/*4*/);
        lv_style_set_pad_ver(box_def_style, 0/*2*/);
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

void lv_box_hide_all()
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

lv_obj_t* lv_box_create_canvas(lv_obj_t* box)
{
    lv_coord_t w = 120 - 2; // lv_obj_get_width(box);
    lv_coord_t h = 120 - 2; // lv_obj_get_height(box);

    static lv_color_t buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(118, 118)];
   

    lv_obj_t* canvas = lv_canvas_create(box);
    lv_obj_set_pos(canvas, 0, 0);
    lv_obj_set_size(canvas, w, h);
    //lv_obj_center(canvas);
    lv_canvas_set_buffer(canvas, buf, w, h, LV_IMG_CF_TRUE_COLOR);
    //lv_canvas_fill_bg(canvas, lv_color_hex(0xFF00FF), LV_OPA_60);
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);
    //lv_canvas_draw_arc(canvas, w / 2, h / 1, w / 2 - 10, 0, 360, 0);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.radius = 10;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
    rect_dsc.border_width = 2;
    rect_dsc.border_opa = LV_OPA_90;
    rect_dsc.border_color = lv_color_white();
    rect_dsc.shadow_width = 5;
    rect_dsc.shadow_ofs_x = 5;
    rect_dsc.shadow_ofs_y = 5;    

    lv_canvas_draw_rect(canvas, 0, 0, 118, 118, &rect_dsc);

    lv_obj_t* label = lv_label_create(box);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_label_set_text(label, "Test");

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
    case LIFT_vs_DRAG:
        info->box_type = type;
        info->content = lv_label_create(box);
        lv_obj_align(info->content, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        lv_obj_set_style_text_font(info->content, font_body, 0);
        lv_label_set_text(info->content, "1234.56");
        break;
    case COMPASS:
        info->box_type = type;
        info->content = lv_box_create_canvas(box);
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
