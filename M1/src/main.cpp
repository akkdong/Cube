#if 0

#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>

#include "device_defines.h"
#include "bsp.h"

#include "lv_hal_porting.h"
#include "lv_app.h"

//
//
//

static app_conf_t app_conf;


//
//
//

void power_event_cb(lv_event_t* evt)
{
	lv_event_code_t code = lv_event_get_code(evt);
	if (code == LV_EVENT_RELEASED)
	{
		Serial.println("Turn of board power!!!");
		bsp_power_on(false);
	}
}


//
//
//

void setup() 
{
  // 
  bsp_init();
  bsp_power_on(true);

  //
  lv_init();  
  lv_hal_setup();

  //
  app_config_init(&app_conf);
  app_init();

  bsp_lcd_backlight(true);
  lv_hal_loop();
}

void loop() 
{
}

#endif