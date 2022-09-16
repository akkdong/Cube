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
#include "LocationParser.h"
#include "Beeper.h"

#define VFILTER_HARINAIR_KF2     1
#define VFILTER_HARINAIR_KF4d    2
#define VFILTER_ROBIN_KF         3

#define USE_KALMAN_FILTER        VFILTER_HARINAIR_KF2

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
#include "VarioFilter_HarInAirKF2.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
#include "VarioFilter_HarInAirKF4d.h"
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
#include "VarioFilter_RobinKF.h"
#else
#error "Invalid vario kalman-filter"
#endif


////////////////////////////////////////////////////////////////////////////////////////
// class Application

class Application
{
public:
    Application();

public:
    void        begin();
    void        end();

    void        update();

protected:
    void        init_config(app_conf_t* conf);
    
protected:
    //
    const lv_font_t * font_large;
    const lv_font_t * font_normal;
    const lv_font_t * font_small;

    app_conf_t*     app_conf = NULL;

    lv_obj_t*       app_annun;
    lv_obj_t*       app_page;

    //
    Variometer      vario;
    LocationParser  locParser;
    Beeper          beeper;

    #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
    VarioFilter_HarInAirKF2     varioFilter;
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
    VarioFilter_HarInAirKF4d    varioFilter;
    #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
    VarioFilter_RobinKF         varioFilter;
    #endif    
};


#endif // __APPLICATION_H__
