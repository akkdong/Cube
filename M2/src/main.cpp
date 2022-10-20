#include <Arduino.h>
#include <Wire.h>

#include "board.h"
#include "i2s_driver.h"
#include "adc_driver.h"

#include "tca9554.h"
#include "bme280.h"
#include "htu21d.h"
#include "es8311.h"

#include "lvgl_porting/lv_port_disp.h"
#include "lvgl_porting/lv_port_indev.h"
#include "lvgl_porting/lv_port_fs.h"

/*

void setup() 
{
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_fs_init(); 
  lv_port_tick_init();
}

void loop() 
{
  vTaskDelay((10) / portTICK_PERIOD_MS);
  lv_task_handler();
}


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

TCA9554       expander(TCA9554A_ADDR, Wire);
Bme280TwoWire	baro;
HTU21D        ht;
ES8311        codec;

uint32_t      lastTick;


void setup()
{
  //
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 33, -1);
  Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
  Serial.println("M2 H/W Test");

  adc_init();

  //
  expander.setOutput(0b10110000);
  expander.setConfig(0b00001111);
  Serial.println("turn-on LCD backlight");
  Serial.printf("Input: %02X\r\n", expander.getInput());
  Serial.printf("Output: %02X\r\n", expander.getOutput());
  Serial.printf("Polarity: %02X\r\n", expander.getPolarity());
  Serial.printf("Config: %02X\r\n", expander.getConfig());

	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary, &Wire);
	baro.setSettings(varioSettings());	  
  Serial.println("begin barometer");

  ht.begin();
  Serial.println("begin humidity & temperature");

  codec.begin((es_i2s_fmt_t)AUDIO_HAL_16K_SAMPLES);
  bsp_i2s_init(I2S_NUM_0, 16000);

  //
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_tick_init();

  //
  lv_obj_t* label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "M2 H/W Test");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_42, LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), LV_STATE_DEFAULT);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);


  //
  Serial.printf("Total heap: %d\r\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());  

  lastTick = millis();
}

void loop()
{
  while (Serial2.available())
  {
    int ch = Serial2.read();
    Serial.write(ch);
  }

  uint32_t tick = millis();
  if (tick - lastTick < (1000 * 2))
  {
    #if 0
    uint16_t x, y;
    if(touch.getPosition(&x, &y)) 
      Serial.printf("Pos: %d, %d\r\n", x, y);

    delay(20);
    #endif
  }
  else
  {
    lastTick = tick;

    float t = baro.getTemperature();
    float p = baro.getPressure();
    Serial.printf("BARO: %u, %f, %f\r\n", tick, t, p);

    ht.measure();
    Serial.printf("HT: %f, %f\r\n", ht.getTemperature(), ht.getHumidity());

    uint16_t voltage = adc_get_voltage();
    Serial.printf("BAT: %d\r\n", voltage);
  }
}
