#include <Arduino.h>
#include <M5EPD.h>
#include "font/binaryttf.h"
#include "logger.h"
#include "NmeaParser.h"


// bsp.h
//
void bsp_hal_init();



M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas output(&M5.EPD);
NmeaParser nmea;

void setup() 
{
  // 
  bsp_hal_init();

  //
  canvas.loadFont(binaryttf, sizeof(binaryttf));
  canvas.createCanvas(960, 80);
  canvas.createRender(64, 256);
  canvas.createRender(32, 256);
  canvas.setTextSize(64);
  canvas.setTextColor(0);
  canvas.setTextDatum(TL_DATUM);
  canvas.fillRect(20, 16, 660, 64, 15);
  canvas.drawString("Cube Variometer", 20, 16);

  canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);

  output.createCanvas(960, 450);
  output.setTextSize(10);
  output.setTextColor(15);
  output.println("NMEA log..");

  //
  nmea.begin();
}

void loop() 
{
  while (Serial1.available())
  {
    int ch = Serial1.read();
    Serial.write(ch);

    int type = nmea.update(ch);
    /*
    if (type == 1)
    {
      time_t date = nmea.getDateTime();
      tm* _tm = localtime(&date);

      output.printf("[%04d/%02d/%02d %02d:%02d:%02d]\n",
        _tm->tm_year + 1970, _tm->tm_mon + 1, _tm->tm_mday,
        _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
      output.printf("GPS: %f %f %f %f\n",
        nmea.getLatitude(), nmea.getLongitude(),
        nmea.getSpeed(), nmea.getCourse());
    }
    else if (type == 2)
    {
      output.printf("VARIO: %f %f %f\n",
        nmea.getPressure(), nmea.getTemperature(), nmea.getVerticalSpeed());
    }
    else if (type == 3)
    {
      uint8_t key, state;
      key = nmea.getLastKey(&state);
      output.printf("KEY: %d(%s)\n", key, state == 1 ? "ON" : "OFF");
    }

    if (type != 0)
      output.pushCanvas(0, 540 - 450, UPDATE_MODE_GL16);
    */
  }

  if (M5.BtnP.pressedFor(2000))
  {
    canvas.clear();
    canvas.setTextSize(32);
    canvas.setTextColor(15);
    canvas.drawString("Power Off!!", 20, 16);
    canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
    delay(1000);

    M5.disableEPDPower();
    M5.disableEXTPower();
    M5.disableMainPower();

    // ???
    esp_deep_sleep_start();
    while (1);
  }

  M5.update();
}
