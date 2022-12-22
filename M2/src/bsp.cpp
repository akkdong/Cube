// bsp.cpp
//

#include <Arduino.h>
#include <Wire.h>

#include "board.h"
#include "bsp.h"
#include "config.h"

#include "tca9554.h"
#include "bme280.h"
#include "htu21d.h"
#include "es8311.h"
#include "BLEVario.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include "SPIFFS.h"
#include "SdCard.h"


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

Bme280TwoWire	baro;
HTU21D          ht;
ES8311          codec;

BLEVario        bleDevice;



//
//
//

void bsp_hal_init()
{
    //
    USBSerial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, 33, -1);
    Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
    USBSerial.println("M2 Variometer");

    // io-expander default setttings
    exio.setOutput(0b10110000);
    exio.setConfig(0b00001111);
    //
    codec.codec_config(AUDIO_HAL_24K_SAMPLES);
    codec.codec_set_voice_volume(62);
    #if DEBUG
    uint16_t id;
    uint8_t version;
    codec.read_chipid(id, version);
    USBSerial.printf("Codec ID: %04X, version: %02X\r\n", id, version);
    #endif


	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary, &Wire);
	baro.setSettings(varioSettings());

    // setup humidity & tempearture
    ht.begin();

	// mount SPIFFS : ...
	SPIFFS.begin();  

	// check sd-card & ready for use
	SD_CARD.begin();

	if (SD_CARD.valid())
	{
    #if 0
		FirmwareUpdater fu;

		if (fu.checkUpdate())
		{
			//
			fu.performUpdate(display);
			//
			ESP.restart();
		}		
		// else go down
    #endif
	}
	else
	{
		USBSerial.printf("SD_CARD is invalid!!\r\n");
	}
}

void bsp_gui_init()
{
    lv_init();

    lv_port_disp_init();
    lv_port_indev_init();
    lv_port_tick_init();   
}

void bsp_power_on(bool on)
{

}

void bsp_lcd_backlight(bool on)
{

}

void bsp_update()
{
	#if 1
	static uint32_t lastTick = millis();
	uint32_t tick = millis();
	if (tick - lastTick >= 2)
	{
		lv_timer_handler();
		lastTick = tick;
	}
	#else
	lv_timer_handler();
	delay(2);
	#endif
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
