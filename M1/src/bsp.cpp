// bsp.cpp
//

#include <Arduino.h>
#include <lvgl.h>
#include "lv_disp.h"

#include "device_defines.h"
#include "bsp.h"
#include "Bme280.h"
#include "FT6206.h"
#include "SineGenerator.h"


Bme280TwoWire	baro;
SineGenerator	beep;
FT6206			touch;



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
	lv_timer_handler();
	delay(5);
}
