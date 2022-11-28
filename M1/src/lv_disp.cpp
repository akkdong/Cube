// lv_hal_porting.c
//

#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>

#include "device_defines.h"
#include "bsp.h"
#include "logger.h"
#include "FT6206.h"
#include "lv_disp.h"
#include "app.h"

#include "TaskBase.h"


//
//
//

Arduino_DataBus* bus = new Arduino_ESP32SPI(LCD_RS, LCD_CS, LCD_CLK, LCD_DO, LCD_DI);
Arduino_GFX* gfx = new Arduino_ST7796(bus, LCD_RST, /* rotation */ 3);

lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf;
lv_color_t* frame_buffer[2];
lv_disp_drv_t disp_drv;
lv_indev_drv_t touch_drv;
lv_indev_drv_t keypad_drv;




///////////////////////////////////////////////////////////////////
// class Display

extern "C" void app_main();

class Display : public TaskBase
{
public:
	Display() : TaskBase("Display", 4* 1024, 1), updateNow(false) {}

public:
	void flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
		#if 0
		x = area->x1;
		y = area->y1;
		w = (area->x2 - area->x1 + 1);
		h = (area->y2 - area->y1 + 1);
		memcpy(frame_buffer[1], frame_buffer[0], sizeof(lv_color_t) * LCD_WIDTH * LCD_HEIGHT);
		lv_disp_flush_ready(disp);
		#else
		_disp = disp;
		_area = area;
		_color_p = color_p;
		#endif

		updateNow = true;
	}

protected:
	void TaskProc() override {
		while (true)
		{
			if (updateNow) 
			{
				#if 0
				for (lv_coord_t i = 0; i < h; i++)
				{
					lv_color_t* buf = frame_buffer[1];
					lv_coord_t _y = y + i;
					gfx->draw16bitRGBBitmap(x, _y, (uint16_t *)(buf + _y * LCD_WIDTH + x), w, 1);
					taskYIELD();
				}
				#else
				uint32_t tick = millis();
				uint32_t w = (_area->x2 - _area->x1 + 1);
				uint32_t h = (_area->y2 - _area->y1 + 1);

				gfx->draw16bitRGBBitmap(_area->x1, _area->y1, (uint16_t *)&_color_p->full, w, h);

				lv_disp_flush_ready(_disp);
				LOGi("flush: %u", millis() - tick);				
				#endif

				updateNow = false;
			} 
			else 
			{
				delay(1);
			}
		}
	}

protected:
	lv_coord_t x, y, w, h;

	lv_disp_drv_t* _disp;
	const lv_area_t* _area;
	lv_color_t* _color_p;

	volatile bool 		updateNow;
};

static Display display;



//
// lvgl-callbacks
//

// Display flushing
static void cube_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
	uint32_t tick = millis();
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);

	#if (LV_COLOR_16_SWAP != 0)
	gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
	#else
	gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
	#endif

	lv_disp_flush_ready(disp);
	//LOGi("flush: %u: %d,%d,%d,%d", millis() - tick, area->x1, area->y1, area->x2, area->y2);
}

// Display flushing
static void cube_disp_flush_ex(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
	display.flush(disp, area, color_p);
}

// Read the touch-panel
extern FT6206 touch;
static void cube_touch_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    Point point = touch.getPoint();

    if( !point.z )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        // Set the coordinates : rotation(3) -> 270 degree
        data->point.x = LCD_WIDTH - point.y;
        data->point.y = point.x;
    }
}

// button
typedef struct _key_map
{
	uint8_t		pin;
	uint8_t		active; // 0: ACTIVE_LOW, others: ACTIVE_HIGH
	uint32_t	key;
} key_map;

static key_map _m1[] =
{
	{ BTN_SEL, 		0, 	LV_KEY_ENTER },
	{ BTN_LEFT, 	1, 	LV_KEY_LEFT },
	{ BTN_RIGHT, 	1, 	LV_KEY_RIGHT },
	{ BTN_UP, 		1, 	LV_KEY_UP },
	{ BTN_DOWN, 	1, 	LV_KEY_DOWN },
};

static lv_indev_state_t cube_keypad_state(key_map* kmap, lv_indev_data_t * data)
{
	data->key = kmap->key;
	data->state = LV_INDEV_STATE_RELEASED;

	int state = digitalRead(kmap->pin);
	if ((kmap->active == 0 && state == 0) || (kmap->active != 0 && state != 0))
		data->state = LV_INDEV_STATE_PRESSED;

	return data->state;
}

static void cube_keypad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
	static int keyNum = 0;

	cube_keypad_state(&_m1[keyNum], data);

	keyNum = (keyNum + 1) % (sizeof(_m1) / sizeof(_m1[0]));
	data->continue_reading = (keyNum == 0) ? false : true;
}



//
//
//

void lv_disp_init()
{
	gfx->begin();
	gfx->fillScreen(BLACK); 
    bsp_lcd_backlight(true);

	int w = gfx->width();
	int h = gfx->height();

	#ifdef ESP32
	//disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * w * 10, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	frame_buffer[0] = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * w * h);
	frame_buffer[1] = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * w * h);
	#else
	disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * w * 10);
	#endif
	if (/*disp_draw_buf*/ frame_buffer[0] && frame_buffer[1])
	{
		lv_disp_draw_buf_init(&draw_buf, frame_buffer[0], frame_buffer[1], w * h);

		// Initialize the display
		lv_disp_drv_init(&disp_drv);
		disp_drv.hor_res = w;
		disp_drv.ver_res = h;
		disp_drv.flush_cb = cube_disp_flush;
		disp_drv.draw_buf = &draw_buf;
		lv_disp_t* disp = lv_disp_drv_register(&disp_drv);

		// Initialize input device driver: touch
		lv_indev_drv_init(&touch_drv);
		touch_drv.type = LV_INDEV_TYPE_POINTER;
		touch_drv.read_cb = cube_touch_read;
		lv_indev_t* touch = lv_indev_drv_register(&touch_drv);

		// Initialize input device driver: button
		lv_indev_drv_init(&keypad_drv);
		keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
		keypad_drv.read_cb = cube_keypad_read;
		lv_indev_t* keypad = lv_indev_drv_register(&keypad_drv);

		//display.createPinnedToCore(0);
		bsp_set_drivers(disp, touch, keypad);

		LOGi("Display initialized!!");
	}
	else
	{
		LOGe("LVGL disp_draw_buf allocate failed!");
	}
}

#if __REMOVED__
void lv_hal_loop()
{
	while(1)
	{
		//
		app_update();

		//
	    lv_timer_handler();
		delay(10);
	}
}
#endif