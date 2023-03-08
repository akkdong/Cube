#include <Arduino.h>
#include "DacESP32.h"
#include "bme280.h"

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

Bme280TwoWire	baro;


DacESP32 dac1(DAC_CHANNEL_1);
DacESP32 dac2(DAC_CHANNEL_2);

HardwareSerial host(0);
HardwareSerial gps(1);

void setup() 
{
  Serial.begin(115200);
  Wire.begin(10, 13);

  host.begin(115200, /*config*/ SERIAL_8N1, /*rx*/ 37, /*tx*/ 39);
  gps.begin(9600, /*config*/ SERIAL_8N1, /*rx*/ 2, /*tx*/ 1);

	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary);
	baro.setSettings(varioSettings());  

  Serial.println();
  Serial.print("Sketch started. Generating ~1000Hz Sinus signal on GPIO (Pin) numbers: ");
  Serial.println(DAC_CHANNEL_1_GPIO_NUM);
  Serial.print(" and ");
  Serial.println(DAC_CHANNEL_2_GPIO_NUM);

  // outputs a sinus signal with frequency ~1000Hz and max. amplitude (default)
  //dac1.outputCW(1000);
  // outputs a sinus signal with frequency ~1000Hz, max. amplitude (default) and 180° phaseshift
  //dac2.outputCW(1000, DAC_CW_SCALE_1, DAC_CW_PHASE_180);  
}

void loop() 
{
  while (gps.available())
  {
    int ch = gps.read();
    host.write(ch);
    Serial.write(ch);
  }

  static uint32_t lastTick = millis();
  if (millis() - lastTick > 1000)
  {
    float t = baro.getTemperature();
    float p = baro.getPressure();
    host.printf("Temp: %f, Pressure: %f\r\n", t, p);
    Serial.printf("Temp: %f, Pressure: %f\r\n", t, p);

    lastTick = millis();
  }
}
