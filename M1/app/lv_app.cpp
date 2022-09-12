// lv_app.cpp
//

#include <lvgl.h>
#include "bsp.h"
#include "logger.h"
#include "lv_app.h"
#include "lv_page.h"


//
//
//

static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;  

static const lv_font_t * font_large;
static const lv_font_t * font_normal;
static const lv_font_t * font_small;

//
// Pages
//  +--Page
//      +--Boxes
//           +--Box
//

// box
//      title
//      desc
//      content

    
lv_box_group_item_t page_1[] = {
    {
        ALTITUDE_GROUND, 0, 0, 180, 96, LV_BORDER_SIDE_FULL
    },
    {
        ALTITUDE_AGL, 0, 96, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        TIME_FLIGHT, 0, 192, 180, 96, LV_BORDER_SIDE_FULL
    },
    {
        SPEED_GROUND, 300, 0, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        SPEED_VERTICAL, 300, 96, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        LIFT_vs_DRAG, 300, 192, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        COMPASS, 180, 0, 120, 120, LV_BORDER_SIDE_NONE
    },
    {
        VSPEED_BAR, 180, 120, 120, 168, LV_BORDER_SIDE_NONE
    },
    {
        END_OF_BOX
    }
};


//
//
//

static void power_event_cb(lv_event_t* evt)
{
	lv_event_code_t code = lv_event_get_code(evt);
	if (code == LV_EVENT_RELEASED)
	{
		LOGv("Turn of board power!!!\n");
		bsp_power_on(false);
	}
}



//
//
//

void app_config_init(app_conf_t* conf)
{

}

void app_init()
{
    font_large = &lv_font_montserrat_48;
    font_normal = LV_FONT_DEFAULT;
    font_small = &lv_font_montserrat_12;

	#if 1
    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);	
	#endif

	//
	/*
	lv_obj_t* scrn = lv_scr_act();
	lv_obj_set_style_bg_color(scrn, lv_color_hex(0x343247), 0);

	lv_obj_t* btn_power = lv_btn_create(scrn);
	lv_obj_set_style_text_font(scrn, font_normal, 0);
	lv_obj_align(btn_power, LV_ALIGN_CENTER, 0, 0);
	

	lv_obj_set_height(btn_power, LV_SIZE_CONTENT);
	lv_obj_add_event_cb(btn_power, power_event_cb, LV_EVENT_ALL, NULL);

	lv_obj_t* label = lv_label_create(btn_power);
	lv_label_set_text(label, "Power Off");
	lv_obj_center(label);
	*/

    //
    //lv_box_init();

    //
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    // annunciator height = 32
    // page height = 320 - 32 = 288
    // one box height = 288 / 3 = 96

    lv_obj_t* ann = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(ann, 4, 0);
    lv_obj_set_style_pad_ver(ann, 0, 0);
    lv_obj_set_pos(ann, 0, 0);
    lv_obj_set_size(ann, 480, 32);
    lv_obj_set_style_radius(ann, 0, 0);
    lv_obj_set_style_border_width(ann, 2, 0);
    lv_obj_set_style_border_color(ann, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_color(ann, lv_color_hex(0x343247), 0);
    //lv_obj_set_style_border_side(ann, LV_BORDER_SIDE_BOTTOM, 0);

    lv_obj_t* page = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(page, 0, 0);
    lv_obj_set_style_pad_ver(page, 0, 0);
    lv_obj_set_pos(page, 0, 32);
    lv_obj_set_size(page, 480, 320 - 32);
    //lv_obj_set_style_radius(page, 0, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    //lv_obj_set_style_border_color(page, lv_color_hex(0x343247), 0);
    //lv_obj_set_style_border_side(page, LV_BORDER_SIDE_NONE, 0);  


    // page
    //      box position/dimention
    //      box type: { title, sub-title, content }
    lv_page_create(page, page_1);



    /*
    lv_obj_t* compass = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(compass, 0, 0);
    lv_obj_set_style_pad_ver(compass, 0, 0);
    lv_obj_set_pos(compass, 0, 32);
    lv_obj_set_size(compass, 120, 120);
    lv_obj_set_style_radius(compass, 6, 0);
    lv_obj_set_style_border_width(compass, 1, 0);

    lv_obj_t* vario = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(vario, 0, 0);
    lv_obj_set_style_pad_ver(vario, 0, 0);
    lv_obj_set_pos(vario, 0, 32 + 120);
    lv_obj_set_size(vario, 120, 320 - 32 - 120);
    lv_obj_set_style_radius(vario, 6, 0);
    lv_obj_set_style_border_width(vario, 1, 0);

    //
    int x = 120;
    int y = 32;
    int w = 180;
    int h = 96;

    lv_obj_t* box = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(box, 4, 0);
    lv_obj_set_style_pad_ver(box, 2, 0);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, w, h);

    lv_obj_t* label = lv_label_create(box);
    lv_label_set_text(label, "Speed");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    label = lv_label_create(box);
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_label_set_text(label, "Km/h");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

    label = lv_label_create(box);
    lv_label_set_text(label, "64.2");
    lv_obj_set_style_text_font(label, font_large, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);    

    //
    y += h;

    box = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(box, 4, 0);
    lv_obj_set_style_pad_ver(box, 2, 0);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, w, h);

    label = lv_label_create(box);
    lv_label_set_text(label, "Altitude");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    label = lv_label_create(box);
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_label_set_text(label, "m");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

    label = lv_label_create(box);
    lv_label_set_text(label, "2832");
    lv_obj_set_style_text_font(label, font_large, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);    

    //
    y += h;

    box = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(box, 4, 0);
    lv_obj_set_style_pad_ver(box, 2, 0);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, w, h);

    label = lv_label_create(box);
    lv_label_set_text(label, "Pressure");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    label = lv_label_create(box);
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_label_set_text(label, "hPa");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

    label = lv_label_create(box);
    lv_label_set_text(label, "1013.4");
    lv_obj_set_style_text_font(label, font_large, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);        


    //
    x += 180;
    y = 32;

    box = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(box, 4, 0);
    lv_obj_set_style_pad_ver(box, 2, 0);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, w, h);

    label = lv_label_create(box);
    lv_label_set_text(label, "Speed V.");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    label = lv_label_create(box);
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_label_set_text(label, "m/s");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

    label = lv_label_create(box);
    lv_label_set_text(label, "5.6");
    lv_obj_set_style_text_font(label, font_large, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);       
    */
}

