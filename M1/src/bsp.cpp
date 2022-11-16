// bsp.cpp
//

#include <Arduino.h>
#include <lvgl.h>
#include "lv_disp.h"
#include "device_defines.h"
#include "bsp.h"
#include "ble_vario.h"

#include "Bme280.h"
#include "FT6206.h"
#include "SineGenerator.h"
#include "BLEVario.h"


Bme280TwoWire	baro;
SineGenerator	beep;
FT6206			touch;

BLEVario 		bleDevice;

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
		.filter = Bme280Filter::Off,
		.standbyTime = Bme280StandbyTime::Ms0_5			
	};
}




//
//
//

void bsp_hal_init()
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

	bleDevice.begin();
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

void bsp_gui_init()
{
	//
	lv_init();

	lv_disp_init();
}



//
//
//

void bsp_update()
{
	static uint32_t lastTick = millis();
	uint32_t tick = millis();
	if (tick - lastTick >= 5)
	{
		lv_timer_handler();
		lastTick = tick;
	}

    // forward serial to bluetooth and reverse : debug purpose only
    while (Serial.available())
    {
        int ch = Serial.read();
        bleDevice.write(ch);
    }

    while (bleDevice.available())
    {
        int ch = bleDevice.read();
        Serial.write(ch);
    }	
}



//
//
//

static lv_disp_t*	cube_disp = NULL;
static lv_indev_t*	cube_touch = NULL;
static lv_indev_t*	cube_keypad = NULL;

void bsp_set_drivers(lv_disp_t* disp, lv_indev_t* touch, lv_indev_t* keypad)
{
	cube_disp = disp;
	cube_touch = touch;
	cube_keypad = keypad;
}

void bsp_regiter_keypad_receiver(lv_obj_t* obj)
{
	if (cube_keypad != NULL)
	{
		lv_group_t* grp = lv_group_create();
		lv_group_add_obj(grp, obj);

		lv_indev_set_group(cube_keypad, grp);	
	}
}



//
//
//

bool ble_isConnected()
{
	return bleDevice.isConnected();
}

int  ble_writeBuffered(uint8_t ch)
{
	return bleDevice.writeBuffered(ch);
}

void ble_flush()
{
	bleDevice.flush();
}

size_t ble_press(uint8_t key)
{
	return bleDevice.press(key);
}

size_t ble_release(uint8_t key)
{
	return bleDevice.release(key);
}
