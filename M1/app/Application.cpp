// Application.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <sys/time.h>
#include "device_defines.h"
#include "bsp.h"
#include "utils.h"
#include "ble_vario.h"
#include "timezone.h"

#include "Application.h"
#include "Beeper.h"

#include "startupWindow.h"
#include "flightWindow.h"


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

extern "C" void page_event_clicked(lv_event_t* event)
{
    LOGi("Page Event: %d", event->code);
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

static Tone melodyStart[] =
{
    { NOTE_C5, 400, 500 },
    { NOTE_E5, 400, 500 },
    { NOTE_G5, 400, 500 },
};


#if 0
extern const lv_img_dsc_t paper_plane;
extern const lv_img_dsc_t bluetooth;
extern const lv_img_dsc_t map_marker;
extern const lv_img_dsc_t volume;


extern "C" bool get_imgfont(const lv_font_t * font, void * img_src, uint16_t len, uint32_t unicode, uint32_t unicode_next)
{
    if (unicode == 0xF001)
    {
        memcpy(img_src, &bluetooth, sizeof(lv_img_dsc_t));
        return true;
    }
    else if (unicode == 0xF002)
    {
        memcpy(img_src, &map_marker, sizeof(lv_img_dsc_t));
        return true;
    }
    else if (unicode == 0xF003)
    {
        memcpy(img_src, &volume, sizeof(lv_img_dsc_t));
        return true;
    }
    else if (unicode == 0xF004)
    {
        memcpy(img_src, &paper_plane, sizeof(lv_img_dsc_t));
        return true;
    }

    return false;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// class Application implementation

Application::Application() 
    : varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE)
    , keyPad(this)
    , bt_lock_state(0)
    , gps_fixed(false)
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

#if 0
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

    // annunciator icons
    // logo, gps, bluetooth, beep          time

    lv_obj_t* clock = lv_label_create(ann);
    lv_obj_set_style_text_font(clock, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(clock, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(clock, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_label_set_text(clock, "");

    /*
    lv_obj_t* icon_logo = lv_img_create(ann);
    lv_img_set_src(icon_logo, &paper_plane);
    
    //lv_img_buf_set_palette(&paper_plane, 0, lv_color_hex(0x000000));
    //lv_img_buf_set_palette(&paper_plane, 1, lv_color_hex(0xFFFFFF));
    lv_obj_align(icon_logo, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* icon_bluetooth = lv_img_create(ann);
    lv_img_set_src(icon_bluetooth, &bluetooth);
    lv_obj_align(icon_bluetooth, LV_ALIGN_LEFT_MID, 24, 0);

    lv_obj_t* icon_gps = lv_img_create(ann);
    lv_img_set_src(icon_gps, &map_marker);
    lv_obj_align(icon_gps, LV_ALIGN_LEFT_MID, 48, 0);

    lv_obj_t* icon_volume = lv_img_create(ann);
    lv_img_set_src(icon_volume, &volume);
    lv_obj_align(icon_volume, LV_ALIGN_LEFT_MID, 72, 0);
    */

    //
    lv_font_t* img_font = lv_imgfont_create(16, get_imgfont);
    img_font->fallback = &lv_font_montserrat_16;

    lv_obj_t* status = lv_label_create(ann);
    lv_obj_set_style_text_font(status, img_font, 0);
    lv_obj_set_style_text_color(status, lv_color_hex(0xFF0020), 0);
    lv_obj_align(status, LV_ALIGN_LEFT_MID, 0, 0);
    lv_label_set_text(status, "\uF004 \uF001 \uF002 \uF003"); // EF 80 84 20 EF 80 81 20 ...

    lv_obj_t* page = lv_obj_create(scr);
    lv_obj_set_style_pad_hor(page, 0, 0);
    lv_obj_set_style_pad_ver(page, 0, 0);
    lv_obj_set_pos(page, 0, 32);
    lv_obj_set_size(page, 480, 320 - 32);
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_bg_color(page, lv_color_hex(0xE0E000), 0);

    bsp_regiter_keypad_receiver(page);
    lv_obj_add_event_cb(page, page_event_cb, LV_EVENT_KEY, 0);
    lv_obj_add_event_cb(page, page_event_clicked, LV_EVENT_CLICKED, 0);

    // page
    //      box position/dimention
    //      box type: { title, sub-title, content }
    lv_page_create(page, page_1);



    app_annun = ann;
    app_page = page;

    app_clock = clock;
    //app_bluetooth = icon_bluetooth;
    //app_gps = icon_gps;
    //app_volume = icon_volume;
#else
    Screen* screen = Screen::instance();
    screen->activateWindow(new StartupWindow);
#endif

    //
    //
    //
    setTimeZone(9 * -3600, 0);

    //
    // start-vario
    //   make-top-menu
    //   load-scren -> show active-screen
    //   initialize: batery, vario
    //      --> update vario/device state
    //
    // 

    #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
    varioFilter.begin(40.0f, 1000000.0f, 0);
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
    varioFilter.begin(500.0f, 8000.0f, 1.0f, 0);
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d

    // injects additional uncertainty depending on magnitude of acceleration
    // helps respond quickly to large accelerations while heavily filtering
    // in low acceleration situations.  Range : 0.5 - 1.5
    #define KF_ACCEL_VARIANCE_DEFAULT   100     // 50 ~ 150
    #define KF_ADAPT_DEFAULT            10     // 50 ~ 150

    varioFilter.begin(KF_ACCEL_VARIANCE_DEFAULT * 1000.0f, KF_ADAPT_DEFAULT / 100.0f, 0, 0, 0);
    #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
    varioFilter.Configure(30.0f, 4.0f, altitude);
    #endif    

    vario.begin(CreateBarometer(), &varioFilter);
    locParser.begin(CreateLocationDataSource());
    beeper.begin(CreateTonePlayer());
    keyPad.begin(CreateKeypadInput());

    beeper.playMelody(melodyStart, sizeof(melodyStart) / sizeof(melodyStart[0]));

    //
    tick_update_time = get_tick();
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
    
    update_time();

    #if 0
    if (app_conf->dirty)
    {
        lv_page_update(app_page);
        app_conf->dirty = 0;
    }
    #else
    static uint32_t lastTick = get_tick();
    uint32_t tick = get_tick();
    if (tick - lastTick > 500)
    {
        Window* active = Screen::instance()->peekWindow();
        if (active)
            active->update();

        lastTick = tick;
    }
    #endif

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

    // button processing
    keyPad.update();

    if (locParser.availableLocation())
    {
        LOGd("[GPS] %f,%f %f", locParser.getLongitude(), locParser.getLatitude(), locParser.getAltitude());

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
        app_conf->speedVertActive = vario.getVelocity();

        vSpeed += app_conf->speedVertActive;
        //LOGi("%f, %f", app_conf->altitudeBaro * 100.0f , app_conf->speedVertActive * 100.0f);

        /*if (!ble_isConnected())*/
            beeper.setVelocity(app_conf->speedVertActive);

        //tick[count] = millis();
        count += 1;
        if (count * (1000 / SENSOR_UPDATE_FREQUENCY) >= VARIOMETER_SENTENCE_DELAY)
        {
            app_conf->speedVertLazy = vSpeed / count;
            app_conf->dirty = 1;

            vSpeed = 0;
            count = 0;
            //lastTick = millis();
        }

        //
        if (varioNmea.checkInterval())
        {
            float height = locParser.isFixed() ? app_conf->altitudeGPS : -1;
            float velocity = app_conf->speedVertLazy;
            float temperature = app_conf->temperature;
            float pressure = app_conf->pressure; // to hPa

            varioNmea.begin(height, velocity, temperature, pressure, -1);
        }
    }


    // vario-sentense available?
    if ((bt_lock_state == 0 && varioNmea.available()) || (bt_lock_state == 1))
    {
        bt_lock_state = 1;

        while (varioNmea.available())
        {
            int ch = varioNmea.read();
            if (ch < 0)
                break;

            ble_writeBuffered(ch);
            //Serial.write(ch);

            if (ch == '\n')
            {
                bt_lock_state = 0;
                break;
            }
        }

        if (bt_lock_state == 0 /*|| bleDevice.uartBufferIsFull()*/)
            ble_flush();
    }    

    // nmea-sentense avaialble?
    if ((bt_lock_state == 0 && locParser.availableNmea()) || (bt_lock_state == 2))
    {
        bt_lock_state = 2;

        while (locParser.availableNmea())
        {
            int ch = locParser.readNmea();
            if (ch < 0)
                break;

            ble_writeBuffered(ch);
            //Serial.write(ch);

            if (ch == '\n')
            {
                bt_lock_state = 0;
                break;
            }
        }

        if (bt_lock_state == 0 /*|| bleDevice.uartBufferIsFull()*/)
            ble_flush();
    }  
}

void Application::OnPressed(uint8_t key) 
{
    LOGv("Key pressed: %02X", key);
    #if ENABLE_BLE_KEYBOARD
    if (key != KEY_ENTER)
        ble_press(key);
    #else
    Window* active = Screen::instance()->peekWindow();
    if (active)
        active->onKeyDown(key);
    #endif
}

void Application::OnLongPressed(uint8_t key) 
{
    LOGv("Key long-pressed: %02X", key);
    if (key == KEY_ENTER)
    {
        LOGi("Turn-off Variometer!!");
        bsp_power_on(false);
    }
    else
    {
        Window* active = Screen::instance()->peekWindow();
        if (active)
            active->onKeyLongDown(key);
    }

}

void Application::OnReleased(uint8_t key) 
{
    LOGv("Key released: %02X", key);
    #if ENABLE_BLE_KEYBOARD
    if (key != KEY_ENTER)
        ble_release(key);
    #else
    Window* active = Screen::instance()->peekWindow();
    if (active)
        active->onKeyUp(key);
    #endif
}  

void Application::update_time()
{
    uint32_t tick = get_tick();
    if (tick_update_time - tick > 1000)
    {
        //
        bool fixed = locParser.isFixed();
        if ((!gps_fixed && fixed) || (gps_fixed && fixed))
        {
            gps_fixed = fixed;
            if (gps_fixed)
                setDeviceTime(locParser.getDateTime());
        }

        //
        #if 0
        time_t t = time(NULL) /*+ 9 * 60 * 60*/;
        struct tm* _tm = localtime(&t);
        
        char sz[32];
        sprintf(sz, "%d:%02d:%02d", _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
        lv_label_set_text(app_clock, sz);
        #endif

        tick_update_time = tick;
    }
}
