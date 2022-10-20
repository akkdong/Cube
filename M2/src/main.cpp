#include <Arduino.h>
#include <Wire.h>

#include "board.h"
#include "tca9554.h"
#include "bme280.h"
#include "htu21d.h"

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

uint32_t      lastTick;


void setup()
{
  //
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 33, -1);
  Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
  Serial.println("M2 H/W Test");

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

  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_tick_init();

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
  }
}
