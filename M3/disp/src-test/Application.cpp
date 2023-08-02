// Application.cpp
//

#include <Arduino.h>
#include "bsp.h"
#include "logger.h"
#include "Application.h"
#include "Widgets.h"
#include "Window.h"
#include "font/binaryttf.h"
#include "ImageResource.h"
#include "KeypadInput.h"


///////////////////////////////////////////////////////////////////////////////////////
// class Application

Application::Application() 
    : Display(&EPD)
    , KEY(this)
    , DEV(Serial1)
    , DBG(Serial)
{
}

void Application::initBoard()
{
    // GPIOs
    pinMode(KEY_POWER_MAIN, OUTPUT);
    pinMode(KEY_POWER_EPD, OUTPUT);
    pinMode(KEY_POWER_EXT, OUTPUT);
    pinMode(KEY_LEFT, INPUT);
    pinMode(KEY_PUSH, INPUT);
    pinMode(KEY_RIGHT, INPUT);

    // hold main power
    digitalWrite(KEY_POWER_MAIN, 1);
    // turn-on power for peripherals
    digitalWrite(KEY_POWER_EPD, 1);
    digitalWrite(KEY_POWER_EXT, 1);
    delay(1000);

    // Communications
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, UART_DEVICE_RX, UART_DEVICE_TX);
    Wire.begin(TOUCH_SDA, TOUCH_SCL, (uint32_t) 400000U);
    SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS_SD);
}

void Application::begin()
{
    //
    EPD.begin(M5EPD_SCK_PIN, M5EPD_MOSI_PIN, M5EPD_MISO_PIN, M5EPD_CS_PIN, M5EPD_BUSY_PIN);
    EPD.Clear(true);
    EPD.SetRotation(M5EPD_Driver::ROTATE_0);
    //
    TP.SetRotation(GT911::ROTATE_0);
    TP.begin(TOUCH_SDA, TOUCH_SCL, TOUCH_INT);
    //
    SD.begin(4, SPI, 20000000);    
    // battery begin
    BAT.begin();
    //
    KEY.begin(CreateKeypadInput());
    //
    //TH.Begin();


    // startup
    Display.loadFont(binaryttf, sizeof(binaryttf));
    Display.createCanvas(M5EPD_PANEL_W, M5EPD_PANEL_H);
    Display.createRender(32);
    Display.createRender(70);
    Display.createRender(80);

    // activate main-window
    MainWindow* pMainWnd = new MainWindow(&Display);
    Scrn.activateWindow(pMainWnd);


    //
    //
    //

   // mode = VARIO_MODE
   //
   // contextPtr = new DeviceContext;
   // loadPref(contextPtr);
   

   // create task
   // FlightComputer
   // Variometer
   
   //screenPtr = new Screen(this);
   //screenPtr->activateWindow(new StartupWindow);
   //

   // setTimeZone()

   // nmeaParser
   // button
   // battery
   // touch
   // ht
   // rtc

    //
    NMEA.begin();

    //
    disableCore0WDT();

    //
    msgQueue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreatePinnedToCore(ScreenTask, "US", 4096, this, 2, NULL, 0);    
    xTaskCreatePinnedToCore(DeviceTask, "DEV", 2048, this, 0, NULL, 0);    
}

void Application::update()
{

    // read serial (NEMA)
    // parse nmea
    // if gps-fixed
    //   update device-state
    //   update canvas(screen)
    // if vario-fixed
    //   update device-sate
    //   update canvas(screen)
    // 

    bool engMode = false;
    Stream& input = engMode ? Serial : Serial1;
    while (false && input.available())
    {
        int ch = input.read();
        Serial.write(ch);

        int type = NMEA.update(ch);
        //
        if (type == 1)
        {
            if (NMEA.isFixed())
            {
                // fixed
                time_t date = NMEA.getDateTime();
                tm* _tm = localtime(&date);

                Serial.printf("[%04d/%02d/%02d %02d:%02d:%02d]\r\n",
                _tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday,
                _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
                Serial.printf("GPS: %f %f %f %f\r\n",
                NMEA.getLatitude(), NMEA.getLongitude(),
                NMEA.getSpeed(), NMEA.getTrack());

                DevContext.varioState.speedGround = NMEA.getSpeed();
                //boxRep[BOX_SPEED].f = NMEA.getSpeed();
                //xQueueSend(_msgQueue, &boxRep[BOX_SPEED].id, 5);

                DevContext.varioState.altitudeGPS = NMEA.getAltitude();
                //boxRep[BOX_GPS_ALTITUDE].f = NMEA.getAltitude();
                //xQueueSend(_msgQueue, &boxRep[BOX_GPS_ALTITUDE].id, 5);

                DevContext.varioState.heading = NMEA.getTrack();
                //boxRep[BOX_HEADING].f = NMEA.getTrack();
                //xQueueSend(_msgQueue, &boxRep[BOX_HEADING].id, 5);

                // send gps-update message
            }
            else
            {
                // unfixed
            }
        }
        else if (type == 2)
        {
            Serial.printf("VARIO: %f %f %f\r\n",
            NMEA.getPressure(), NMEA.getTemperature(), NMEA.getVerticalSpeed());

            DevContext.varioState.speedVertActive = NMEA.getVerticalSpeed();
            //boxRep[BOX_VSPEED].f = NMEA.getVerticalSpeed();
            //xQueueSend(_msgQueue, &boxRep[BOX_VSPEED].id, 5);

            DevContext.varioState.temperature = NMEA.getTemperature();
            //boxRep[BOX_TEMPERATURE].f = (int)NMEA.getTemperature();
            //xQueueSend(_msgQueue, &boxRep[BOX_TEMPERATURE].id, 5);

            DevContext.varioState.pressure = NMEA.getTemperature();
            //boxRep[BOX_PRESSURE].f = NMEA.getPressure();
            //xQueueSend(_msgQueue, &boxRep[BOX_PRESSURE].id, 5);
        }
        else if (type == 3)
        {
            uint8_t key, state;
            key = NMEA.getLastKey(&state);
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

    //
    //KEY.update();

    /*
    if (BTN[1].pressedFor(2000))
    {
        Display.clear();
        int x = (M5EPD_PANEL_W - 128) / 2;
        int y = (M5EPD_PANEL_H - 128) / 2;
        Display.pushImage(x, y, 128, 128, ImageResource_biglogo_wolf_howl_128x128);
        Display.pushCanvas(0, 0, UPDATE_MODE_GLD16);
        //EPD.UpdateFull(UPDATE_MODE_GC16);

        delay(1000);

        digitalWrite(M5EPD_EXT_PWR_EN_PIN, 0);
        digitalWrite(M5EPD_EPD_PWR_EN_PIN, 0);        
        delay(100);
        digitalWrite(M5EPD_MAIN_PWR_PIN, 0);
        while(1);
    }
    */

   delay(1);
}



// KeypadCallback
void Application::onPressed(uint8_t key)
{
    //uint32_t message = (key << 16) | 1;
    //xQueueSend(queue, &message, 5);
    Serial.printf("Key Pressed: %d\r\n", key);
}

void Application::onLongPressed(uint8_t key)
{
    //uint32_t message = (key << 16) | 2;
    //xQueueSend(queue, &message, 5);
    Serial.printf("Key Long-Pressed: %d\r\n", key);

    if (key == KEY_PUSH)
    {
        uint32_t msg = (9999 << 16);
        Serial.printf("Send Message: %x\r\n", msg);
        xQueueSend(msgQueue, &msg, 5);
    }
}

void Application::onReleased(uint8_t key)
{
    //uint32_t message = (key << 16) | 0;
    //xQueueSend(queue, &message, 5);
    Serial.printf("Key Released: %d\r\n", key);
}



void Application::ScreenTask(void* param)
{
    Application* pThis = (Application *)param;    
    pThis->ScreenTask();
    
    vTaskDelete(NULL);
}

void Application::DeviceTask(void* param)
{
    Application* pThis = (Application *)param;
    pThis->DeviceTask();

    vTaskDelete(NULL);
}

void Application::ScreenTask()
{
    //
    Window* active = Scrn.getActiveWindow();
    active->update(&DevContext);
    active->draw(/*true*/);


    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    bool exit = false;

    while (!exit)
    {
        uint32_t msg;
        if (xQueueReceive(msgQueue, &msg, 10))
        {
            uint16_t code = (uint16_t)(msg >> 16);
            uint16_t data = (uint16_t)(msg & 0x00FF);
            Serial.printf("MSG: code(%d), data(%d)\r\n", code, data);

            switch (code)
            {
            case 9999:
                exit = true;
                break;
            }

        }        
    }

    //
    {
        Display.clear();
        int x = (LCD_WIDTH - 128) / 2;
        int y = (LCD_HEIGHT - 128) / 2;
        Display.pushImage(x, y, 128, 128, ImageResource_biglogo_wolf_howl_128x128);
        Display.pushCanvas(0, 0, UPDATE_MODE_GLD16);

        delay(1000);

        digitalWrite(KEY_POWER_EXT, 0);
        digitalWrite(KEY_POWER_EPD, 0);        
        delay(100);
        digitalWrite(KEY_POWER_MAIN, 0);
        while(1);
    }
}

void Application::DeviceTask()
{
    #define LOOP_PERIOD         (100)
    #define CHECK_INTERVAL(x)   ((x)/(LOOP_PERIOD))

    const TickType_t xDelay = LOOP_PERIOD / portTICK_PERIOD_MS;
    int BAT_delayCount = 0;
    int TH_delayCount = 0;

    while (1)
    {
        //
        if (++BAT_delayCount > CHECK_INTERVAL(200))
        {
            if (BAT.update() > 0)
            {
                float volt = BAT.getVoltage();
                DevContext.deviceState.batteryPower = volt;
                //SendMessage(msgQueue, code, param);
                LOGv("Battery: %.2fv", volt);
            }

            BAT_delayCount = 0;
        }

        //
        if (++TH_delayCount > CHECK_INTERVAL(1000))
        {
            if (TH.UpdateData() == 0)
            {
                float tem = TH.GetTemperature();
                float hum = TH.GetRelHumidity();
                DevContext.deviceState.temperature = tem;
                DevContext.deviceState.humidity = hum;
                //SendMessage(msgQueue, code, param);
                LOGv("Temperature: %.2f*C, Humidity: %.2f%%", tem, hum);
            }

            TH_delayCount = 0;
        }

        //
        //TP

        //
        KEY.update();

        vTaskDelay(xDelay);
    }
}
