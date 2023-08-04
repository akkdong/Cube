#include <Arduino.h>
#include <M5EPD.h>
#include "board.h"
#include "font/binaryttf.h"
#include "logger.h"
#include "NmeaParser.h"
#include "Digit.h"


class InfoBox
{
public:
  InfoBox(int x, int y, int w, int h);

  enum DataType {
    DT_NONE,
    DT_FLOAT,
    DT_INT,
    DT_DATETIME
  };

  struct Content {
    DataType type;
    uint32_t flag;
    union {
      float* pf;
      int* pi;
      time_t* pt;
    };
  };

  void init(const char* title, float* pfValue, uint32_t flag);
  void init(const char* title, int* piValue, uint32_t flag);
  void init(const char* title, time_t* ptValue, uint32_t flag);

  void draw(m5epd_update_mode_t mode);

protected:
  M5EPD_Canvas* mCanvasPtr;
  int m_x, m_y, m_w, m_h;

  const char* mTitle;
  Content mContent;
};

InfoBox::InfoBox(int x, int y, int w, int h) 
{
  m_x = x;
  m_y = y;
  m_w = w;
  m_h = h;

  mCanvasPtr = new M5EPD_Canvas(&M5.EPD);
  if (mCanvasPtr) {
    mCanvasPtr->createCanvas(w, h);
    mCanvasPtr->setTextSize(32);
    mCanvasPtr->setTextColor(M5EPD_Canvas::G15);      
  }    

  mTitle = nullptr;
  mContent.type = DT_NONE;
}

void InfoBox::init(const char* title, float* pfValue, uint32_t flag)
{
  mTitle = title;
  mContent.type = DT_FLOAT;
  mContent.flag = flag;
  mContent.pf = pfValue;
}

void InfoBox::init(const char* title, int* piValue, uint32_t flag)
{
  mTitle = title;
  mContent.type = DT_FLOAT;
  mContent.flag = flag;
  mContent.pi = piValue;  
}

void InfoBox::init(const char* title, time_t* ptValue, uint32_t flag)
{
  mTitle = title;
  mContent.type = DT_FLOAT;
  mContent.flag = flag;
  mContent.pt = ptValue;  
}


void InfoBox::draw(m5epd_update_mode_t mode)
{
  if (!mCanvasPtr)
    return;

  mCanvasPtr->fillRect(0, 0, m_w, m_h, M5EPD_Canvas::G0);
  mCanvasPtr->drawRect(0, 0, m_w, m_h, M5EPD_Canvas::G15);

  if (mTitle)
  {
    mCanvasPtr->setTextSize(32);
    mCanvasPtr->setTextDatum(TL_DATUM);
    mCanvasPtr->drawString(mTitle, 4, 4);
  }

  if (mContent.type != DT_NONE)
  {
    char sz[32];
    sz[0] = 0;
    switch (mContent.type)
    {
    case DT_FLOAT:
      sprintf(sz, "%.1f", *mContent.pf);
      break;
    case DT_INT:
      sprintf(sz, "%d", *mContent.pi);
      break;
    case DT_DATETIME:
      break;
    }

    if (sz[0] != 0)
    {
      mCanvasPtr->setTextSize(84);
      mCanvasPtr->setTextDatum(CC_DATUM);
      mCanvasPtr->drawString(sz, m_w / 2, m_h / 2 + 16);
    }
  }

  mCanvasPtr->pushCanvas(m_x, m_y, mode);
}


M5EPD_Canvas canvas(&M5.EPD);
InfoBox* boxes[6];
NmeaParser nmea;

enum BoxIdentify {
  BOX_SPEED = 0,
  BOX_GPS_ALTITUDE = 1,
  BOX_HEADING = 2,
  BOX_VSPEED = 3,
  BOX_TEMPERATURE = 4,
  BOX_PRESSURE = 5,
};

struct BoxRepository {
  uint32_t id;
  const char* title;
  union {
    float f;
    int i;
    time_t t;
  };
};

BoxRepository boxRep[] =
{
  { BOX_SPEED, "Speed", 0 },
  { BOX_GPS_ALTITUDE, "Altitude", 0 },
  { BOX_HEADING, "Heading", 0 },
  { BOX_VSPEED, "V. Speedd", 0 },
  { BOX_TEMPERATURE, "Temperature", 0 },
  { BOX_PRESSURE, "Pressure", 0 }
};

QueueHandle_t _UpdateQueue = xQueueCreate(20, sizeof(uint32_t));

void UpdateScreen(void* param)
{
  while(1)
  {
    uint32_t cmd;
    if (xQueueReceive(_UpdateQueue, &cmd, 10))
    {
      if (cmd < sizeof(boxes) / sizeof(boxes[0]))
      {
        // redraw box
        boxes[cmd]->draw(UPDATE_MODE_DU);
      }
      else
      {
        if (cmd == 99)
          M5.EPD.UpdateFull(UPDATE_MODE_GC16);
        else // if (cmd == 100)
          break; // exit loop
      }
    }
  }

  // shutdown
  M5.EPD.Clear(true);

  canvas.clear();
  canvas.setTextSize(32);
  canvas.setTextColor(15);
  canvas.setTextDatum(TL_DATUM);
  canvas.drawString("Power Off!!", 20, 16);
  canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
  delay(1000);

  M5.disableEPDPower();
  M5.disableEXTPower();
  M5.disableMainPower();

  // ???
  esp_deep_sleep_start();
  while (1);  

  vTaskDelete(NULL);
}

void bsp_hal_init();

void setup() 
{
  // 
  bsp_hal_init();

  //
  #define HEADER_H      (60)

  canvas.loadFont(binaryttf, sizeof(binaryttf));
  canvas.createCanvas(960, HEADER_H);
  canvas.createRender(84, 256);
  canvas.createRender(64, 256);
  canvas.createRender(32, 256);
  canvas.setTextSize(64);
  canvas.setTextColor(M5EPD_Canvas::G0);
  canvas.setTextDatum(CL_DATUM);
  canvas.fillRect(0, 0, 960, HEADER_H, M5EPD_Canvas::G10);
  canvas.drawString("Cube", 12, HEADER_H / 2);
  canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);

  for (int i = 0; i < sizeof(boxes) / sizeof(boxes[0]); i++)
  {
    int x = (i % 3) * 320;
    int y = (i / 3) * 240 + HEADER_H;
    boxes[i] = new InfoBox(x, y, 320, 240);
    if (!boxes[i])
    {
      Serial.printf("Canvas creation failed!!: #%d\r\n", i);
      while (1);
    }

    switch (i)
    {
    case 0:
      boxes[i]->init(boxRep[i].title, &boxRep[i].f, 0);
      break;
    case 1:
      boxes[i]->init(boxRep[i].title, &boxRep[i].f, 0);
      break;
    case 2:
      boxes[i]->init(boxRep[i].title, &boxRep[i].f, 0);
      break;
    case 3:
      boxes[i]->init(boxRep[i].title, &boxRep[i].f, 0);
      break;
    case 4:
      boxes[i]->init(boxRep[i].title, &boxRep[i].f, 0);
      break;
    case 5:
      boxes[i]->init(boxRep[i].title, &boxRep[i].f, 0);
      break;
    }

    boxes[i]->draw(UPDATE_MODE_NONE); // UPDATE_MODE_NONE, UPDATE_MODE_DU
  }

  M5.EPD.UpdateFull(UPDATE_MODE_GLR16);

  //
  nmea.begin();

  //
  disableCore0WDT();
  xTaskCreatePinnedToCore(UpdateScreen, "US", 4096, NULL, 1, NULL, 0);  
}




void loop() 
{
  while (Serial1.available())
  {
    int ch = Serial1.read();
    Serial.write(ch);

    int type = nmea.update(ch);
    //
    if (type == 1)
    {
      if (nmea.isFixed())
      {
        // fixed
        time_t date = nmea.getDateTime();
        tm* _tm = localtime(&date);

        Serial.printf("[%04d/%02d/%02d %02d:%02d:%02d]\r\n",
          _tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday,
          _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
        Serial.printf("GPS: %f %f %f %f\r\n",
          nmea.getLatitude(), nmea.getLongitude(),
          nmea.getSpeed(), nmea.getTrack());

        boxRep[BOX_SPEED].f = nmea.getSpeed();
        xQueueSend(_UpdateQueue, &boxRep[BOX_SPEED].id, 5);

        boxRep[BOX_GPS_ALTITUDE].f = nmea.getAltitude();
        xQueueSend(_UpdateQueue, &boxRep[BOX_GPS_ALTITUDE].id, 5);

        boxRep[BOX_HEADING].f = nmea.getTrack();
        xQueueSend(_UpdateQueue, &boxRep[BOX_HEADING].id, 5);
      }
      else
      {
        // unfixed
      }
    }
    else if (type == 2)
    {
      Serial.printf("VARIO: %f %f %f\r\n",
        nmea.getPressure(), nmea.getTemperature(), nmea.getVerticalSpeed());

      boxRep[BOX_VSPEED].f = nmea.getVerticalSpeed();
      xQueueSend(_UpdateQueue, &boxRep[BOX_VSPEED].id, 5);

      boxRep[BOX_TEMPERATURE].f = (int)nmea.getTemperature();
      xQueueSend(_UpdateQueue, &boxRep[BOX_TEMPERATURE].id, 5);

      boxRep[BOX_PRESSURE].f = nmea.getPressure();
      xQueueSend(_UpdateQueue, &boxRep[BOX_PRESSURE].id, 5);
    }
    else if (type == 3)
    {
      uint8_t key, state;
      key = nmea.getLastKey(&state);
      Serial.printf("KEY: %d(%s)\r\n", key, state == 0 ? "OFF" : (state == 1 ? "ON" : "LONG"));

      if (key == 0xB0 && state == 2) // KEY_ENTER(UP), LONG
      {
        Serial1.println("MUTE 0");
        Serial.println("MUTE 0");
      }
      else if (key == 0xB1 && state == 2) // KEY_ESCAPE(DOWN), LONG
      {
        Serial1.println("MUTE 1");
        Serial.println("MUTE 1");
      }
    }

    //if (type != 0)
    //  output.pushCanvas(0, 540 - 450, UPDATE_MODE_GL16);

    //
  }

  if (M5.BtnP.pressedFor(2000))
  {
    uint32_t cmd = 100;
    xQueueSend(_UpdateQueue, &cmd, 0);
    delay(1000);
  }

  //if (M5.BtnR.pressedFor(500))
  //{
  //  uint32_t cmd = 99;
  //  xQueueSend(_UpdateQueue, &cmd, 5);
  //}

  M5.update();
}
