#include <Arduino.h>
#include <Wire.h>

#include "board.h"
#include "config.h"
#include "logger.h"
#include "timezone.h"
#include "utils.h"
#include "i2s_driver.h"
#include "adc_driver.h"
#include "bsp.h"

#include "tca9554.h"
#include "bme280.h"
#include "htu21d.h"
#include "es8311.h"
#include "BLEVario.h"

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

//Bme280TwoWire	baro;
//HTU21D        ht;
//ES8311        codec;

//BLEVario 		  bleDevice;


//
//Variometer                  vario;
//LocationParser              locParser;
//VarioLogger                 igc;
//VarioSentence               varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE);
//Beeper                      beeper;
//BatteryVoltage              battery;
//AGL                         agl;

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
VarioFilter_HarInAirKF2     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
//VarioFilter_HarInAirKF3     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
VarioFilter_HarInAirKF4d    varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
VarioFilter_RobinKF         varioFilter;
#endif

//
//int                         bt_lock_state = 0; // 0: unlocked, 1: locked_by_vario, 2: locked_by_gps
//uint8_t                     tick_update_time;

//bool                        gps_fixed = false;


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
        vTaskDelay(pdMS_TO_TICKS(5));
        lv_task_handler();        
    }
}



//
//
//

void setup()
{
    //
    bsp_hal_init();
    bsp_power_on(true);

    bsp_gui_init();
    bsp_lcd_backlight(true);


    //
    //TaskWatchdog::begin(1000);
    //TaskWatchdog::add(NULL);  

    //
    DeviceRepository& repo = DeviceRepository::instance();
    repo.reset();
    repo.loadPref();      

    //
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

    // timer(task) handler
    bsp_update();

    //
    app.update();
}
