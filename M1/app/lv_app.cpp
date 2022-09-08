// lv_app.cpp
//

#include <lvgl.h>
#include "bsp.h"
#include "logger.h"
#include "lv_app.h"

//
//
//

static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;  

static const lv_font_t * font_large;
static const lv_font_t * font_normal;


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

	#if 0
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
	lv_obj_t* scrn = lv_scr_act();
	lv_obj_set_style_bg_color(scrn, lv_color_hex(0x343247), 0);

	lv_obj_t* btn_power = lv_btn_create(scrn);
	lv_obj_set_style_text_font(scrn, font_large, 0);
	lv_obj_align(btn_power, LV_ALIGN_CENTER, 0, 0);

	lv_obj_set_height(btn_power, LV_SIZE_CONTENT);
	lv_obj_add_event_cb(btn_power, power_event_cb, LV_EVENT_ALL, NULL);

	lv_obj_t* label = lv_label_create(btn_power);
	lv_label_set_text(label, "Power Off");
	lv_obj_center(label);
}

