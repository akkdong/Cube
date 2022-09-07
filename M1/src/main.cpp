#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>

#include "device_defines.h"
#include "Bme280.h"
#include "FT6206.h"
#include "SineGenerator.h"

//
#if 0
class Display : public Arduino_GFX
{
public:
  Display(Arduino_DataBus* bufPtr, int8_t rst, uint8_t rotation);


public:
  void        begin();

  static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
  static void touch_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data );

protected:
  uint32_t    width;
  uint32_t    height;

  lv_disp_draw_buf_t draw_buf;
  lv_color_t *disp_draw_buf;
  lv_disp_drv_t disp_drv;
  lv_indev_drv_t indev_drv;
};
#endif

Arduino_DataBus* bus = new Arduino_ESP32SPI(LCD_RS, LCD_CS, LCD_CLK, LCD_DO, LCD_DI);
Arduino_GFX* gfx = new Arduino_ST7796(bus, LCD_RST, /* rotation */ 3);

lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf;
lv_disp_drv_t disp_drv;
lv_indev_drv_t indev_drv;

Bme280TwoWire	  baro;
SineGenerator 	beep;
FT6206			    touch;


//
// baro-settings
//

static Bme280Settings varioSettings() 
{
	return {
		.mode = Bme280Mode::Normal,
		.temperatureOversampling = Bme280Oversampling::X2,
		.pressureOversampling = Bme280Oversampling::X16,
		.humidityOversampling = Bme280Oversampling::Off,
		.filter = Bme280Filter::X16,
		.standbyTime = Bme280StandbyTime::Ms0_5			
	};
}


//
// lvgl-callbacks
//

// Display flushing
void cube_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = (area->x2 - area->x1 + 1);
   uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
   gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
   gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

   lv_disp_flush_ready(disp);
}

// Read the touch-panel
void cube_touch_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
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


//
//
//

void bsp_init()
{
	// initialize pin-mode
	pinMode(POWER_ON, OUTPUT);
	pinMode(BTN_SEL, INPUT);
	pinMode(BTN_LEFT, INPUT);
	pinMode(BTN_RIGHT, INPUT);
	pinMode(BTN_UP, INPUT);	
	pinMode(BTN_DOWN, INPUT);

	pinMode(LCD_BL, OUTPUT);

	// setup-serial
	Serial.begin(115200);
	Serial2.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
	Serial.println("CUBE M1 started!!");

	// setup-wire
	Wire.begin(I2C0_SDA, I2C0_SCL);

	// setup-pwm
	beep.begin(SineGenerator::USE_CHANNEL_1);
	
	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary);
	baro.setSettings(varioSettings());		
	
	// setup-touch
	touch.begin(); 
}

void bsp_power_on(bool on)
{
	digitalWrite(POWER_ON, on ? HIGH : LOW);
}

void bsp_lcd_backlight(bool on)
{
	digitalWrite(LCD_BL, on ? HIGH : LOW);	
}


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

void app_init()
{
	// initialize app-configuration
	// ...

	// ...
}


void app_disp_init()
{
	int w = gfx->width();
	int h = gfx->height();

	#ifdef ESP32
	disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * w * 10, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	#else
	disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * w * 10);
	#endif
	if (disp_draw_buf)
	{
		lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, w * 10);

		// Initialize the display
		lv_disp_drv_init(&disp_drv);
		disp_drv.hor_res = w;
		disp_drv.ver_res = h;
		disp_drv.flush_cb = cube_disp_flush;
		disp_drv.draw_buf = &draw_buf;
		lv_disp_drv_register(&disp_drv);

		// Initialize input device driver
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		indev_drv.read_cb = cube_touch_read;
		lv_indev_drv_register(&indev_drv);

		Serial.println("Display initialized!!");
	}
	else
	{
		Serial.println("LVGL disp_draw_buf allocate failed!");
	}
}

void app_page_create()
{
	static lv_style_t style_text_muted;
	static lv_style_t style_title;
	static lv_style_t style_icon;
	static lv_style_t style_bullet;  

	static const lv_font_t * font_large;
	static const lv_font_t * font_normal;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

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

	//
	lv_obj_t* power = lv_btn_create(lv_scr_act());
	lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

	lv_obj_set_height(power, LV_SIZE_CONTENT);
	lv_obj_add_event_cb(power, power_event_cb, LV_EVENT_ALL, NULL);

	lv_obj_t* label = lv_label_create(power);
	lv_label_set_text(label, "Power Off");
	lv_obj_center(label);
}

//
//
//

void setup() {
  // 
  bsp_init();
  bsp_power_on(true);

  // 
	gfx->begin();
	gfx->fillScreen(BLACK); 

  bsp_lcd_backlight(true);

  //
  lv_init();  

  app_init();
  app_disp_init();
  app_page_create();
}

void loop() {
  lv_timer_handler();
}
