// Application.h
//

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <lvgl.h>
#include "bsp.h"
#include "logger.h"
#include "app.h"

#include "DeviceRepository.h"
#include "Variometer.h"
#include "VarioSentence.h"
#include "LocationParser.h"
#include "Beeper.h"
#include "keypad.h"
#include "BatteryVoltage.h"
#include "agl.h"
#include "VarioLogger.h"
//#include "Accumulator.h"
#include "BluetoothManager.h"
#include "VariometerEx.h"
#include "BeeperEx.h"

#define VFILTER_HARINAIR_KF2     1
#define VFILTER_HARINAIR_KF3     2
#define VFILTER_HARINAIR_KF4d    3
#define VFILTER_ROBIN_KF         4

#define USE_KALMAN_FILTER        VFILTER_HARINAIR_KF3

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


////////////////////////////////////////////////////////////////////////////////////////
// class Application

class Screen;
class Window;
class Widget;
class StartupWindow;
class FlightWindow;

class Application : public KeypadCallback/*, public TaskBase*/
{
    friend class Window;
    friend class Widget;
    friend class StartupWindow;
    friend class FlightWindow;

public:
    Application();

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
        MSG_UPDATE_NMEA,
        MSG_UPDATE_VARIO,
        MSG_UPDATE_ANNUNCIATOR,

        MSG_START_VARIO,

        MSG_GPS_FIXED,
        MSG_TAKEOFF,
        MSG_LANDING,

        MSG_FLUSH_VARIO_NMEA,
        MSG_FLUSH_GPS_NMEA,

        //MSG_START_FLIGHT, // MSG_TAKEOFF
        //MSG_STOP_FLIGHT, // MSG_LANDING

        MSG_KEY_PRESSED,
        MSG_KEY_LONG_PRESSED,
        MSG_KEY_RELEASED,
    };

    struct Message
    {
        uint16_t    code;
        uint16_t    data;
    };

public:
    void                        begin();
    void                        end();

    void                        update();
    void                        updateOthers();

protected:
    // KeypadCallback
    void                        onPressed(uint8_t key) override;
    void                        onLongPressed(uint8_t key) override;
    void                        onReleased(uint8_t key) override;

    // TaskBase
    //void                      TaskProc() override;

    static void                 onCalibrateAltitude(struct _lv_timer_t * timer);

protected:
    //void                      init_config(app_conf_t* conf);
    void                        update_time();

    void                        updateFlightState();
    void                        checkFlightMode();

    void                        startCircling();
    void                        startGliding();
    void                        stopCircling();
    void                        stopGliding();

    void                        startFlight();
    void                        stopFlight();
    void                        startVario();

    //
    void                        postMessage(uint16_t code, uint16_t data);

    //
    void                        calibrateAltitude();

    void                        onReadyLocationData();

    static void                 FlightComputerTask(void* param);
    static void                 LocationTask(void* param);    
    static void                 VariometerTask(void* param);    

public:
    static CriticalSection      lock;

protected:
    //
    DeviceContext*              contextPtr;
    volatile DeviceMode         mode;

    Screen*                     screenPtr;

    //uint8_t                     deviceMode; // undef, wakeup, vario, vario_and_gp, pref
    //uint8_t                     varioMode; // init, landing, flying, circling

    //    
    Variometer                  vario;
    BeeperEx                    beeper;
    LocationParser              locParser;
    VarioSentence               varioNmea;
    Keypad                      keyPad;
    BatteryVoltage              battery;
    AGL                         agl;
    VarioLogger                 igc;
//  Accumulator                 accumulateSpeed; // speedCalculator;
//  Accumulator                 accumulatePressure;
    BluetoothManager            bt;

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
    #if 0
    int                         bt_lock_state; // 0: unlocked, 1: locked_by_vario, 2: locked_by_gps
    #endif

    uint32_t                    tick_updateTime;
    uint32_t                    tick_updateDisp;
    uint32_t                    tick_stopBase;
    uint32_t                    tick_silentBase;

    bool                        gpsFixed;
    bool                        dispNeedUpdate;

    //
    //uint32_t                    modeTick;

    //
    TaskHandle_t                taskFlightComputer;
    TaskHandle_t                taskLocation;
    TaskHandle_t                taskVariometer;

    EventGroupHandle_t          fcEventGroup;
    QueueHandle_t               mainQueue;

    CriticalSection             contextLock;
};


#endif // __APPLICATION_H__
