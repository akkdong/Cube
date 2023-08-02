// Application.h
//

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <M5EPD.h>
#include <M5EPD_Driver.h>
#include <M5EPD_Canvas.h>

#include "Widgets.h"
#include "Window.h"
#include "Screen.h"
#include "Keypad.h"
#include "NmeaParser.h"
#include "DeviceContext.h"
#include "Battery.h"



/////////////////////////////////////////////////////////////////////////////////
// class Application

class Application : public IKeypadCallback
{
public:
    Application();

public:
    //
    void begin();
    void update();

    //
    static void initBoard();

protected:
    // IKeypadCallback
    virtual void onPressed(uint8_t key);
    virtual void onLongPressed(uint8_t key);
    virtual void onReleased(uint8_t key);

protected:
    static void ScreenTask(void* param);
    static void DeviceTask(void* param);

    void ScreenTask();
    void DeviceTask();


protected:
    // Peripherals
    M5EPD_Driver EPD;
    GT911 TP;
    BM8563 RTC;
    SHT3x TH;
    Keypad KEY;
    BatteryADC BAT;
    Stream& DEV;
    Stream& DBG;

    // Display canvas
    M5EPD_Canvas Display;
    // Window management
    Screen Scrn;

    //
    DeviceContext DevContext;
    NmeaParser  NMEA;

    //
    QueueHandle_t msgQueue;
};


#endif // __APPLICATION_H__
