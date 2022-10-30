// Application.h
//

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <lvgl.h>
#include "bsp.h"
#include "logger.h"
#include "lv_app.h"
#include "lv_page.h"

#include "Variometer.h"
#include "VarioSentence.h"
#include "LocationParser.h"
#include "Beeper.h"
#include "Keypad.h"

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

class Application : public KeypadCallback
{
public:
    Application();

public:
    void        begin();
    void        end();

    void        update();

protected:
    // KeypadCallback
    virtual void OnPressed(uint8_t key);
    virtual void OnLongPressed(uint8_t key);
    virtual void OnReleased(uint8_t key);

protected:
    void        init_config(app_conf_t* conf);
    void        update_time();
    
protected:
    //
    const lv_font_t *           font_large;
    const lv_font_t *           font_normal;
    const lv_font_t *           font_small;

    app_conf_t*                 app_conf;

    lv_obj_t*                   app_annun;
    lv_obj_t*                   app_page;

    lv_obj_t*                   app_clock;
    lv_obj_t*                   app_bluetooth;
    lv_obj_t*                   app_gps;
    lv_obj_t*                   app_volume;

    //
    Variometer                  vario;
    LocationParser              locParser;
    VarioSentence               varioNmea;
    Beeper                      beeper;
    Keypad                      keyPad;

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
    int                         bt_lock_state; // 0: unlocked, 1: locked_by_vario, 2: locked_by_gps
    uint8_t                     tick_update_time;

    bool                        gps_fixed;
};


#endif // __APPLICATION_H__
