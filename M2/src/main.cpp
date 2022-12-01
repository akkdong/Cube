#include <Arduino.h>
#include <Wire.h>

#include "board.h"
#include "config.h"
#include "logger.h"
#include "timezone.h"
#include "utils.h"
#include "i2s_driver.h"
#include "adc_driver.h"
#include "ble_vario.h"

#include "tca9554.h"
#include "bme280.h"
#include "htu21d.h"
#include "es8311.h"
#include "BLEVario.h"


#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"


#include "Variometer.h"
#include "VarioLogger.h"
#include "VarioSentence.h"
#include "LocationParser.h"
#include "Beeper.h"

#include "SPIFFS.h"
#include "SdCard.h"
#include "agl.h"
#include "BatteryVoltage.h"
#include "TaskWatchdog.h"

#include "app/Application.h"


/*

void loop_scan_i2c_devices()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) 
        Serial.print("0");
      Serial.println(address, HEX);

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) 
        Serial.print("0");
      Serial.println(address, HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");

  delay(5000);           // wait 5 seconds for next scan
}


*/



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
HTU21D        ht;
ES8311        codec;

BLEVario 		  bleDevice;


//
Variometer                  vario;
LocationParser              locParser;
VarioLogger                 igc;
VarioSentence               varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE);
Beeper                      beeper;
BatteryVoltage              battery;
AGL                         agl;

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
VarioFilter_HarInAirKF2     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
VarioFilter_HarInAirKF3     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
VarioFilter_HarInAirKF4d    varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
VarioFilter_RobinKF         varioFilter;
#endif

//
int                         bt_lock_state = 0; // 0: unlocked, 1: locked_by_vario, 2: locked_by_gps
uint8_t                     tick_update_time;

bool                        gps_fixed = false;


//
//
//

Application                 app;



//
//
//

void lv_tick_task(void *arg)
{
    while(1) 
    {
        vTaskDelay((10) / portTICK_PERIOD_MS);
        lv_task_handler();        
    }
}



//
//
//

void setup()
{
  //
  USBSerial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 33, -1);
  Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
  USBSerial.println("M2 H/W Test");

  //
  battery.begin();

  // io-expander default setttings
  exio.setOutput(0b10110000);
  exio.setConfig(0b00001111);
  //
  codec.codec_config(AUDIO_HAL_24K_SAMPLES);
  codec.codec_set_voice_volume(78);
  #if DEBUG
  uint16_t id;
  uint8_t version;
  codec.read_chipid(id, version);
  USBSerial.printf("Codec ID: %04X, version: %02X\r\n", id, version);    
  #endif

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
		USBSerial.printf("SD_CARD is invalid!!\n");
	}

	// load last device-context
	SPIFFS.begin();  

	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary, &Wire);
	baro.setSettings(varioSettings());	 
  // setup humidity & tempearture
  ht.begin();
  //
  //codec.begin((es_i2s_fmt_t)AUDIO_HAL_16K_SAMPLES);
  //bsp_i2s_init(I2S_NUM_0, 16000);
  //
  bleDevice.begin();

  //
	//TaskWatchdog::begin(1000);
	//TaskWatchdog::add(NULL);  

  // bsp_gui_init()
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_tick_init();

  //xTaskCreate(lv_tick_task, "lv_tick_task", 4096, NULL, 1, NULL);

  //
  DeviceRepository& repo = DeviceRepository::instance();
  repo.reset();
  repo.loadPref();      

  app.begin();

  #if 0 // MOVED TO LOOP
  while(1)
  {
    //
	  TaskWatchdog::reset();    

    //bsp_update();
    lv_task_handler();
    delay(2);

    // forward serial to bluetooth and reverse : debug purpose only
    while (USBSerial.available())
    {
        int ch = USBSerial.read();
        bleDevice.write(ch);
    }

    while (bleDevice.available())
    {
        int ch = bleDevice.read();
        USBSerial.write(ch);
    }	    

    app_update();
  }
  #endif
}

void loop()
{
  //
  //TaskWatchdog::reset();    

  // bsp_update();
  lv_task_handler();
  delay(2);

  // forward serial to bluetooth and reverse : debug purpose only
  /*
  while (USBSerial.available())
  {
      int ch = USBSerial.read();
      bleDevice.write(ch);
  }

  while (bleDevice.available())
  {
      int ch = bleDevice.read();
      USBSerial.write(ch);
  }	
}
  }	
  */    

  app.update();
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
