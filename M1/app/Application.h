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
#include "Keypad.h"
#include "Battery.h"
#include "Agl.h"
#include "IGCLogger.h"
#include "SpeedCalculator.h"
#include "BluetoothManager.h"


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

class Window;
class Widget;
class StartupWindow;

class Application : public KeypadCallback
{
    friend class Window;
    friend class Widget;
    friend class StartupWindow;

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

public:
    void                        begin();
    void                        end();

    void                        update();

protected:
    // KeypadCallback
    void                        onPressed(uint8_t key) override;
    void                        onLongPressed(uint8_t key) override;
    void                        onReleased(uint8_t key) override;

    static void                 onCalibrateAltitude(struct _lv_timer_t * timer);

protected:
    //void                      init_config(app_conf_t* conf);
    void                        update_time();

    void                        updateFlightState();

    void                        startCircling();
    void                        startGliding();
    void                        stopCircling();
    void                        stopGliding();

    void                        startFlight();
    void                        stopFlight();
    void                        startVario();

    void                        calibrateAltitude();
    
protected:
    //
    DeviceContext*              context;
    DeviceMode                  mode;

    //uint8_t                     deviceMode; // undef, wakeup, vario, vario_and_gp, pref
    //uint8_t                     varioMode; // init, landing, flying, circling

    //
    Variometer                  vario;
    LocationParser              locParser;
    VarioSentence               varioNmea;
    Beeper                      beeper;
    Keypad                      keyPad;
    Battery                     battery;
    AGL                         agl;
    IGCLogger                   igc;
    SpeedCalculator             speedCalculator;
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
};


#endif // __APPLICATION_H__
