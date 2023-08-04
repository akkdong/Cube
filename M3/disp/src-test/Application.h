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
#include "DeviceRepository.h"
#include "Battery.h"
#include "agl.h"
#include "SdCard.h"
#include "CriticalSection.h"

#include "BluetoothManager.h"
#include "VarioLogger.h"
#include "VarioSentence.h"


//
//
//
enum DeviceMode
{
    MODE_INIT,
    MODE_GROUND,
    MODE_FLYING,
    MODE_CIRCLING,
    MODE_GLIDING,
    MODE_SETTING,
};

enum MessageCode {
    MSG_NONE,
    MSG_UPDATE_GPS, // MSG_UPDATE_NMEA
    MSG_UPDATE_VARIO,
    MSG_UPDATE_ANNUNCIATOR,
    MSG_UPDATE_TH,
    MSG_UPDATE_BAT,

    MSG_REDRAW,

    MSG_START_VARIO,

    MSG_GPS_FIXED, // data: fixed(1), unfixed(0)
    MSG_TAKEOFF,
    MSG_LANDING,

    MSG_FLUSH_VARIO_NMEA,
    MSG_FLUSH_GPS_NMEA,

    //MSG_START_FLIGHT, // MSG_TAKEOFF
    //MSG_STOP_FLIGHT, // MSG_LANDING

    MSG_KEY_PRESSED,
    MSG_KEY_LONG_PRESSED,
    MSG_KEY_RELEASED,

    MSG_SHUTDOWN,
};

struct Message
{
    uint16_t    code;
    uint16_t    data;
};


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
    //
    void startVario();

    void startFlight();
    void updateFlightState();
    void checkFlightMode();
    void stopFlight();

    void startCircling();
    void stopCircling();
    void startGliding();
    void stopGliding();

    void sendMessage(uint16_t code, uint16_t data = 0);

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
    DeviceContext* contextPtr;
    NmeaParser  NMEA;
    AGLClass AGL;
    BluetoothManager BT;
    VarioLogger IGC;
    VarioSentence varioNmea;

    //
    volatile DeviceMode mode;

    uint32_t tick_updateTime;
    uint32_t tick_updateDisp;
    uint32_t tick_stopBase;
    uint32_t tick_silentBase;

    bool gpsFixed;
    bool dispNeedUpdate;    

    //
    TaskHandle_t taskScreen;
    TaskHandle_t taskDevice;

    QueueHandle_t msgQueue;

    CriticalSection contextLock;
};


#endif // __APPLICATION_H__
