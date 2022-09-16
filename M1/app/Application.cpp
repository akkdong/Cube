// Application.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include "Application.h"
#include "Beeper.h"
#include "bsp.h"



extern "C" app_conf_t* app_get_conf()
{
    static app_conf_t conf;

    return &conf;
}

extern "C" void app_config_init(app_conf_t* conf)
{
	//
    memset(conf, 0, sizeof(app_conf_t));

    //
    conf->bearing = -1;
}

extern "C" void page_event_cb(lv_event_t* event)
{
    uint32_t key = lv_indev_get_key(lv_indev_get_act());
    LOGi("Page Event: %d, %d", event->code, key);
}


////////////////////////////////////////////////////////////////////////////////////////
//

lv_page_item_t page_1[] = {
    {
        ALTITUDE_GROUND, 0, 0, 180, 96, LV_BORDER_SIDE_FULL
    },
    {
        TRACK_HEADING, 0, 96, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        SPEED_GROUND, 0, 192, 180, 96, LV_BORDER_SIDE_FULL
    },
    {
        ALTITUDE_BARO, 300, 0, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        SENSOR_PRESSURE, 300, 96, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        SPEED_VERTICAL, 300, 192, 180, 96, LV_BORDER_SIDE_FULL 
    },
    {
        COMPASS, 180, 0, 120, 120, LV_BORDER_SIDE_NONE
    },
    {
        VSPEED_BAR, 180, 120, 120, 168, LV_BORDER_SIDE_FULL
    },
    {
        END_OF_BOX
    }
};



////////////////////////////////////////////////////////////////////////////////////////
// class Application implementation

Application::Application()
{
    // ...
}

void Application::begin()
{
    //
    app_conf = app_get_conf();
    app_config_init(app_conf);

    //
    font_large = &lv_font_montserrat_48;
    font_normal = LV_FONT_DEFAULT;
    font_small = &lv_font_montserrat_12;

    //
    lv_box_init();

    //
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    // annunciator height = 32
    // page height = 320 - 32 = 288
    // one box height = 288 / 3 = 96

    lv_obj_t* ann = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(ann, 4, 0);
    lv_obj_set_style_pad_ver(ann, 0, 0);
    lv_obj_set_pos(ann, 0, 0);
    lv_obj_set_size(ann, 480, 32);
    lv_obj_set_style_radius(ann, 0, 0);
    lv_obj_set_style_border_width(ann, 2, 0);
    lv_obj_set_style_border_color(ann, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_color(ann, lv_color_hex(0x343247), 0);
    //lv_obj_set_style_border_side(ann, LV_BORDER_SIDE_BOTTOM, 0);

    lv_obj_t* page = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(page, 0, 0);
    lv_obj_set_style_pad_ver(page, 0, 0);
    lv_obj_set_pos(page, 0, 32);
    lv_obj_set_size(page, 480, 320 - 32);
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_bg_color(page, lv_color_hex(0xE0E0E6), 0);

    bsp_regiter_keypad_receiver(page);
    lv_obj_add_event_cb(page, page_event_cb, LV_EVENT_KEY, 0);

    // page
    //      box position/dimention
    //      box type: { title, sub-title, content }
    lv_page_create(page, page_1);


    app_annun = ann;
    app_page = page;


    //
    // start-vario
    //   make-top-menu
    //   load-scren -> show active-screen
    //   initialize: batery, vario
    //      --> update vario/device state
    //
    // 

    #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
    varioFilter.begin(400.0f, 1000.0f, 1.0f, 0);
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d

    // injects additional uncertainty depending on magnitude of acceleration
    // helps respond quickly to large accelerations while heavily filtering
    // in low acceleration situations.  Range : 0.5 - 1.5
    #define KF_ADAPT			1.0f

    varioFilter.begin(1000.0f * 1, KF_ADAPT, 0, 0, 0);
    #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
    varioFilter.Configure(30.0f, 4.0f, altitude);
    #endif    

    vario.begin(CreateBarometer(), &varioFilter);
    locParser.begin(CreateLocationDataSource());
    beeper.begin(CreateTonePlayer());
}

void Application::end()
{
}

void Application::update()
{
    //
    // application routines
    //
    //
    
    if (app_conf->dirty)
    {
        lv_page_update(app_page);
        app_conf->dirty = 0;
    }

    // reset-watchdog
    // keybd-update
    // vario.avaiable?
    //   update context-data: v-vel, v-vel-statistic, altitude, ....
    //   update beeper
    //   check-sleep?
    //   update vario-sentence : LX..
    //
    // nmea-parser-update
    // nmea-parser.data-ready?
    //   update vario-state (gps relative stubs)
    //   check takeoff or landing
    //     or update flight-state
    // logger-update
    // bt-update
    // battery-update
    // process-key

    locParser.update();
    int varioUpdated = vario.update();
    beeper.update();

    while (locParser.availableNmea())
    {
        int ch = locParser.readNmea();
        //trace_putc(ch);
    }

    if (locParser.availableLocation())
    {
        LOGi("[GPS] %f,%f %f", locParser.getLongitude(), locParser.getLatitude(), locParser.getAltitude());
        app_conf->latitude = locParser.getLatitude();
        app_conf->longitude = locParser.getLongitude();
        app_conf->altitudeGPS = locParser.getAltitude();
        app_conf->speedGround = locParser.getSpeed();
        app_conf->heading = locParser.getHeading();
        app_conf->dirty = 1;

        locParser.resetLocation();
    }

    if (varioUpdated > 0)
    {
        //static uint32_t lastTick = millis();
        //static uint32_t tick[30];
        static uint32_t count = 0;
        static float vSpeed = 0;

        app_conf->altitudeBaro = vario.getAltitudeFiltered();
        app_conf->pressure = vario.getPressure();
        app_conf->temperature = vario.getTemperature();

        //app_conf->speedVertActive = vario.getVelocity();
        float temp = vario.getVelocity();
        vSpeed += temp;
        beeper.setVelocity(temp);

        //tick[count] = millis();
        count += 1;
        if ((count % 25) == 0)
        {
            /*
            LOGi("Update vario-relative stubs: %u", millis() - lastTick);
            LOGi("  %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",
                tick[1] - tick[0],
                tick[2] - tick[1],
                tick[3] - tick[2],
                tick[4] - tick[3],
                tick[5] - tick[4],
                tick[6] - tick[5],
                tick[7] - tick[6],
                tick[8] - tick[7],
                tick[9] - tick[8],
                tick[10] - tick[9],
                tick[11] - tick[10],
                tick[12] - tick[11],
                tick[13] - tick[12],
                tick[14] - tick[13],
                tick[15] - tick[14],
                tick[16] - tick[15],
                tick[17] - tick[16],
                tick[18] - tick[17],
                tick[19] - tick[18],
                tick[20] - tick[19]);
            */

            app_conf->speedVertActive = vSpeed / count;
            app_conf->dirty = 1;

            vSpeed = 0;
            count = 0;
            //lastTick = millis();
        }
    }
}
