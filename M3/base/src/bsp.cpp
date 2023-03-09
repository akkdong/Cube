// bsp.cpp
//

#include <Arduino.h>
#include "TwoWireEx.h"
#include "board.h"
#include "bsp.h"
#include "config.h"
#include "bme280.h"
#include "SPIFFS.h"


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

Stream & SerialDebug 	= Serial;
Stream & SerialHost 	= Serial0;
Stream & SerialGPS 		= Serial1;


Bme280TwoWire	baro;


DacESP32 dac1(DAC_CHANNEL_1);
DacESP32 dac2(DAC_CHANNEL_2);



//
//
//

void bsp_hal_init()
{
    //
    Serial.begin(115200);

	Serial0.begin(115200, UART_HOST_RX, UART_HOST_TX);

	Serial1.setRxBufferSize(1024);
    Serial1.begin(9600, SERIAL_8N1, UART_GPS_RX, UART_GPS_TX);
	
    WireEx.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
    Serial.println("M3 Variometer");

	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary, &WireEx);
	baro.setSettings(varioSettings());

	// mount SPIFFS : ...
	//SPIFFS.begin();  
}

void bsp_gui_init()
{
}

void bsp_power_on(bool on)
{
}

void bsp_lcd_backlight(bool on)
{
}

void bsp_update()
{
}
