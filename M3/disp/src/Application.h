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
#include "Message.h"
#include "Keypad.h"
#include "NmeaParser.h"
#include "DeviceRepository.h"
#include "Battery.h"
#include "agl.h"
#include "CriticalSection.h"

#include "BluetoothManager.h"
#include "VarioLogger.h"
#include "VarioSentence.h"
#include "IGCSentence.h"
#include "CaptivePortal.h"

#include <memory>
#include <algorithm>


//
//
//
enum DeviceMode
{
    MODE_INIT,
    MODE_GROUND,
    MODE_FLYING,
    MODE_CIRCLING,
    MODE_GLIDING
};

// UDPATE FLAG
#define UFLAG_GPS_DATA      0x0001
#define UFLAG_VARIO_DATA    0x0002
#define UFLAG_BAT_DATA      0x0004
#define UFLAG_TH_DATA       0x0008


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

    void sendMessage(uint32_t code, uint32_t data = 0);

    bool isEngineerMode() { return engMode; }
    void enableEngineerMode(bool enable);

    //
    static void initBoard();
    static Application* getApp();

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

    float getGroundLevel(float lat, float lon);

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
    IGCSentence igcSentence;

    std::unique_ptr<CaptivePortal> portalPtr;

    //
    volatile DeviceMode deviceMode;

    uint32_t tick_updateTime;
    uint32_t tick_updateDisp;
    uint32_t tick_stopBase;
    uint32_t tick_silentBase;

    bool gpsFixed;
    bool prepareShutdown;
    bool engMode;
    bool wifiEnabled;

    //
    TaskHandle_t taskScreen;
    TaskHandle_t taskDevice;

    QueueHandle_t msgQueue;

    CriticalSection contextLock;
};


#endif // __APPLICATION_H__
