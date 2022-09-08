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
  bsp_lcd_backlight(true);

  //
  lv_init();  

  app_setup();
  app_create_page();
}

void loop() 
{
  app_loop();
}
