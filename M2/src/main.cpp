#include <Arduino.h>
#include <Wire.h>

#include "board.h"
#include "config.h"
#include "logger.h"
#include "timezone.h"
#include "utils.h"
#include "i2s_driver.h"
#include "adc_driver.h"

#include "tca9554.h"
#include "bme280.h"
#include "htu21d.h"
#include "es8311.h"
#include "BLEVario.h"


#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include "lv_app.h"
#include "lv_page.h"

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

#define VFILTER_HARINAIR_KF2     1
#define VFILTER_HARINAIR_KF3     2
#define VFILTER_HARINAIR_KF4d    3
#define VFILTER_ROBIN_KF         4

#define USE_KALMAN_FILTER        VFILTER_HARINAIR_KF2

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
#include "VarioFilter_HarInAirKF2.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
#include "VarioFilter_HarInAirKF3.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
#include "VarioFilter_HarInAirKF4d.h"
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
#include "VarioFilter_RobinKF.h"
#else
#error "Invalid vario kalman-filter"
#endif

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
//
//

const lv_font_t *           font_large = nullptr;
const lv_font_t *           font_normal = nullptr;
const lv_font_t *           font_small = nullptr;

app_conf_t*                 app_conf = nullptr;

lv_obj_t*                   app_annun = nullptr;
lv_obj_t*                   app_page = nullptr;

lv_obj_t*                   app_clock = nullptr;
lv_obj_t*                   app_bluetooth = nullptr;
lv_obj_t*                   app_gps = nullptr;
lv_obj_t*                   app_volume = nullptr;

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

void app_begin();
void app_end();
void app_update();
void app_update_time();

bool ble_isConnected();
int  ble_writeBuffered(uint8_t ch);
void ble_flush();
size_t ble_press(uint8_t key);
size_t ble_release(uint8_t key);


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
	TaskWatchdog::begin(1000);
	TaskWatchdog::add(NULL);  

  //
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_tick_init();

  //xTaskCreate(lv_tick_task, "lv_tick_task", 4096, NULL, 1, NULL);

  app_begin();

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
}

void loop()
{
  #if 0
  while (Serial2.available())
  {
    int ch = Serial2.read();
    USBSerial.write(ch);
  }

  uint32_t tick = millis();
  if (tick - lastTick < (1000 * 2))
  {
    #if 0
    uint16_t x, y;
    if(touch.getPosition(&x, &y)) 
      USBSerial.printf("Pos: %d, %d\r\n", x, y);

    delay(20);
    #endif
  }
  else
  {
    lastTick = tick;

    float t = baro.getTemperature();
    float p = baro.getPressure();
    USBSerial.printf("BARO: %u, %f, %f\r\n", tick, t, p);

    ht.measure();
    USBSerial.printf("HT: %f, %f\r\n", ht.getTemperature(), ht.getHumidity());

    uint16_t voltage = adc_get_voltage();
    USBSerial.printf("BAT: %d\r\n", voltage);
  }
  #endif
}


//
//
//


lv_page_item_t page_1[] = {
  {
      ALTITUDE_GROUND, 0, 0, 300, 144, LV_BORDER_SIDE_FULL
  },
  {
      TRACK_HEADING, 0, 144, 300, 144, LV_BORDER_SIDE_FULL 
  },
  {
      SPEED_GROUND, 0, 288, 300, 144, LV_BORDER_SIDE_FULL
  },
  {
      ALTITUDE_BARO, 500, 0, 300, 144, LV_BORDER_SIDE_FULL 
  },
  {
      SENSOR_PRESSURE, 500, 144, 300, 144, LV_BORDER_SIDE_FULL 
  },
  {
      SPEED_VERTICAL, 500, 288, 300, 144, LV_BORDER_SIDE_FULL 
  },
  {
      COMPASS, 300, 0, 200, 200, LV_BORDER_SIDE_NONE
  },
  {
      VSPEED_BAR, 300, 200, 200, 232, LV_BORDER_SIDE_FULL
  },
  {
      END_OF_BOX
  }
};

extern const lv_img_dsc_t paper_plane;
extern const lv_img_dsc_t bluetooth;
extern const lv_img_dsc_t map_marker;
extern const lv_img_dsc_t volume;

static Tone melodyStart[] =
{
    { NOTE_C5, 400, 500 },
    { NOTE_E5, 400, 500 },
    { NOTE_G5, 400, 500 },
};


//
//
//

extern "C" app_conf_t* app_get_conf()
{
    static app_conf_t conf;

    return &conf;
}

extern "C" void app_config_init(app_conf_t* conf)
{
	//
    memset(conf, 0, sizeof(app_conf_t));

    //
    conf->bearing = -1;
}


void app_begin()
{
  //
  app_conf = app_get_conf();
  app_config_init(app_conf);

  //
  font_large = &lv_font_montserrat_48;
  font_normal = LV_FONT_DEFAULT;
  font_small = &lv_font_montserrat_12;

  //
  lv_box_init();

  //
  lv_obj_t* scr = lv_obj_create(NULL);
  lv_scr_load(scr);

  // annunciator height = 48
  // page height = 480 - 48 = 432
  // one box height = 432 / 3 = 144

  lv_obj_t* ann = lv_obj_create(scr);
  lv_obj_set_style_pad_hor(ann, 4, 0);
  lv_obj_set_style_pad_ver(ann, 0, 0);
  lv_obj_set_pos(ann, 0, 0);
  lv_obj_set_size(ann, LCD_WIDTH, 48);
  lv_obj_set_style_radius(ann, 0, 0);
  lv_obj_set_style_border_width(ann, 2, 0);
  lv_obj_set_style_border_color(ann, lv_color_hex(0x343247), 0);
  lv_obj_set_style_bg_color(ann, lv_color_hex(0x343247), 0);
  //lv_obj_set_style_border_side(ann, LV_BORDER_SIDE_BOTTOM, 0);


  // annunciator icons
  // logo, gps, bluetooth, beep          time

  lv_obj_t* clock = lv_label_create(ann);
  lv_obj_set_style_text_font(clock, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(clock, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(clock, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_label_set_text(clock, "");

  lv_obj_t* icon_logo = lv_img_create(ann);
  lv_img_set_src(icon_logo, &paper_plane);
  
  //lv_img_buf_set_palette(&paper_plane, 0, lv_color_hex(0x000000));
  //lv_img_buf_set_palette(&paper_plane, 1, lv_color_hex(0xFFFFFF));
  lv_obj_align(icon_logo, LV_ALIGN_LEFT_MID, 0, 0);

  lv_obj_t* icon_bluetooth = lv_img_create(ann);
  lv_img_set_src(icon_bluetooth, &bluetooth);
  lv_obj_align(icon_bluetooth, LV_ALIGN_LEFT_MID, 24, 0);

  lv_obj_t* icon_gps = lv_img_create(ann);
  lv_img_set_src(icon_gps, &map_marker);
  lv_obj_align(icon_gps, LV_ALIGN_LEFT_MID, 48, 0);

  lv_obj_t* icon_volume = lv_img_create(ann);
  lv_img_set_src(icon_volume, &volume);
  lv_obj_align(icon_volume, LV_ALIGN_LEFT_MID, 72, 0);

  lv_obj_t* page = lv_obj_create(scr);
  lv_obj_set_style_pad_hor(page, 0, 0);
  lv_obj_set_style_pad_ver(page, 0, 0);
  lv_obj_set_pos(page, 0, 48);
  lv_obj_set_size(page, LCD_WIDTH, LCD_HEIGHT - 48);
  lv_obj_set_style_border_width(page, 0, 0);
  lv_obj_set_style_bg_color(page, lv_color_hex(0xE0E0E6), 0);

  //bsp_regiter_keypad_receiver(page);
  //lv_obj_add_event_cb(page, page_event_cb, LV_EVENT_KEY, 0);

  // page
  //      box position/dimention
  //      box type: { title, sub-title, content }
  lv_page_create(page, page_1);


  app_annun = ann;
  app_page = page;

  app_clock = clock;
  app_bluetooth = icon_bluetooth;
  app_gps = icon_gps;
  app_volume = icon_volume;


  //
  //
  //
  setTimeZone(9 * -3600, 0);

  //
  // start-vario
  //   make-top-menu
  //   load-scren -> show active-screen
  //   initialize: batery, vario
  //      --> update vario/device state
  //
  // 

  #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
  varioFilter.begin(500.0f, 8000.0f, 0);
  #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
  varioFilter.begin(500.0f, 8000.0f, 1.0f, 0);
  #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d

  // injects additional uncertainty depending on magnitude of acceleration
  // helps respond quickly to large accelerations while heavily filtering
  // in low acceleration situations.  Range : 0.5 - 1.5
  #define KF_ACCEL_VARIANCE_DEFAULT   100     // 50 ~ 150
  #define KF_ADAPT_DEFAULT            10     // 50 ~ 150

  varioFilter.begin(KF_ACCEL_VARIANCE_DEFAULT * 1000.0f, KF_ADAPT_DEFAULT / 100.0f, 0, 0, 0);
  #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
  varioFilter.Configure(30.0f, 4.0f, altitude);
  #endif    

  vario.begin(CreateBarometer(), &varioFilter);
  locParser.begin(CreateLocationDataSource());
  beeper.begin(CreateTonePlayer());

  beeper.playMelody(melodyStart, sizeof(melodyStart) / sizeof(melodyStart[0]));

  //
  tick_update_time = get_tick();
}


void app_end()
{
  // nop
}

void app_update()
{
  // application routines
  //
  //

  app_update_time();

  if (app_conf->dirty)
  {
    lv_page_update(app_page);
    app_conf->dirty = 0;
  }

  // reset-watchdog
  // keybd-update
  // vario.avaiable?
  //   update context-data: v-vel, v-vel-statistic, altitude, ....
  //   update beeper
  //   check-sleep?
  //   update vario-sentence : LX..
  //
  // nmea-parser-update
  // nmea-parser.data-ready?
  //   update vario-state (gps relative stubs)
  //   check takeoff or landing
  //     or update flight-state
  // logger-update
  // bt-update
  // battery-update
  // process-key

  locParser.update();
  int varioUpdated = vario.update();
  beeper.update();

  // UI: update GPS fixed state
  // status.GPS = locParser.isFixed() ? 1 : 0;

  if (locParser.availableLocation())
  {
    LOGd("[GPS] %f,%f %f", locParser.getLongitude(), locParser.getLatitude(), locParser.getAltitude());

    //app_conf->latitudeLast = app_conf->latitude;
    //app_conf->longitudeLast = app_conf->longigute;
    //app_conf->headingLast = app_conf->heading;

    app_conf->latitude = locParser.getLatitude();
    app_conf->longitude = locParser.getLongitude();
    app_conf->speedGround = locParser.getSpeed();
    app_conf->heading = locParser.getHeading();
    app_conf->altitudeGPS = locParser.getAltitude();
    //app_conf->altitudeGround = agl.getGroundLevel(app_conf->latitude, app_conf->longitude);
    //app_conf->altitudeAGL = app_conf->altitudeGPS - app_conf->altitudeGround;
    //app_conf->altitudeRef1 = ...
    //app_conf->timeCurrent = locParser.getDateTime();
    app_conf->dirty = 1;

    //
    // update device-mode : vario_only, vario_gps, setting, simulate, ...
    // update vario-mode: ready, landing, flying, circling, (gliding)
    //

    locParser.resetLocation();
  }

  if (varioUpdated > 0)
  {
    //static uint32_t lastTick = millis();
    //static uint32_t tick[30];
    static uint32_t count = 0;
    static float vSpeed = 0;

    app_conf->altitudeBaro = vario.getAltitudeFiltered();
    //app_conf->altitudeCalibrated = vario.getCalibratedAltitude();
    app_conf->pressure = vario.getPressure();
    app_conf->temperature = vario.getTemperature();
    app_conf->speedVertActive = vario.getVelocity();

    vSpeed += app_conf->speedVertActive;
    //LOGi("%f, %f", app_conf->altitudeBaro * 100.0f , app_conf->speedVertActive * 100.0f);

    //varioCalcualtor.add(app_conf->speedVertActive);
    //
    // update history
    //

    /*if (!ble_isConnected())*/
        beeper.setVelocity(app_conf->speedVertActive);

    //tick[count] = millis();
    count += 1;
    if (count * (1000 / SENSOR_UPDATE_FREQUENCY) >= VARIOMETER_SENTENCE_DELAY)
    {
      app_conf->speedVertLazy = vSpeed / count;
      app_conf->dirty = 1;

      vSpeed = 0;
      count = 0;
      //lastTick = millis();
    }

    //
    if (varioNmea.checkInterval())
    {
      float height = locParser.isFixed() ? app_conf->altitudeGPS : -1;
      float velocity = app_conf->speedVertLazy;
      float temperature = app_conf->temperature;
      float pressure = app_conf->pressure; // to hPa
      //float battery = adc.getVoltage();

      varioNmea.begin(height, velocity, temperature, pressure, -1);
    }
  }

  // check igc-logging
  if (igc.isLogging() && locParser.availableIGC())
  {
    float altitude = vario.getAltitude();
    igc.updateBaroAltitude(altitude);

    while (locParser.availableIGC())
      igc.write(locParser.readIGC());
  }

  // vario-sentense available?
  if ((bt_lock_state == 0 && varioNmea.available()) || (bt_lock_state == 1))
  {
    bt_lock_state = 1;

    while (varioNmea.available())
    {
      int ch = varioNmea.read();
      if (ch < 0)
      break;
      
      ble_writeBuffered(ch); // USBSerial.write(ch);

      if (ch == '\n')
      {
        bt_lock_state = 0;
        break;
      }
    }

    if (bt_lock_state == 0 /*|| bleDevice.uartBufferIsFull()*/)
      ble_flush();
  }    

  // nmea-sentense avaialble?
  if ((bt_lock_state == 0 && locParser.availableNmea()) || (bt_lock_state == 2))
  {
    bt_lock_state = 2;

    while (locParser.availableNmea())
    {
      int ch = locParser.readNmea();
      if (ch < 0)
        break;
      
      ble_writeBuffered(ch); // USBSerial.write(ch)

      if (ch == '\n')
      {
        bt_lock_state = 0;
        break;
      }
    }

    if (bt_lock_state == 0 /*|| bleDevice.uartBufferIsFull()*/)
      ble_flush();
  }

  // battery
  if (battery.update())
  {
    // update UI
    // app_conf.deviceState.batteryPower = battery.getVoltage();
    
  }

  // key(?)
  // ...    
}

void app_update_time()
{
  uint32_t tick = get_tick();
  if (tick_update_time - tick > 1000)
  {
    //
    bool fixed = locParser.isFixed();
    if ((!gps_fixed && fixed) || (gps_fixed && fixed))
    {
      gps_fixed = fixed;
      if (gps_fixed)
        setDeviceTime(locParser.getDateTime());
    }

    //
    time_t t = time(NULL) /*+ 9 * 60 * 60*/;
    struct tm* _tm = localtime(&t);

    char sz[32];
    sprintf(sz, "%d:%02d:%02d", _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
    lv_label_set_text(app_clock, sz);

    tick_update_time = tick;
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
