// main.cpp
//

#include <Arduino.h>
#include "device_defines.h"
#include "logger.h"
#include "lv_app.h"
#include "BLEVario.h"
#include "Variometer.h"
#include "VarioSentence.h"
#include "LocationParser.h"
#include "Beeper.h"
#include "Bme280.h"
#include "FT6206.h"
#include "SineGenerator.h"

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


//
// button
//

typedef struct _key_map
{
	uint8_t		pin;
	uint8_t		active; // 0: ACTIVE_LOW, others: ACTIVE_HIGH
	uint8_t	    key;
    uint8_t     state;  // 0: release, others: press
} key_map;


static key_map _m1[] =
{
	{ BTN_SEL, 		0, 	KEY_RETURN, 0 },
	{ BTN_LEFT, 	1, 	KEY_LEFT_ARROW, 0 },
	{ BTN_RIGHT, 	1, 	KEY_RIGHT_ARROW, 0 },
	{ BTN_UP, 		1, 	KEY_UP_ARROW, 0 },
	{ BTN_DOWN, 	1, 	KEY_DOWN_ARROW, 0 },
};

//
// baro-settings
//

static Bme280Settings varioSettings() 
{
	return {
		.mode = Bme280Mode::Normal,
		.temperatureOversampling = Bme280Oversampling::X2,
		.pressureOversampling = Bme280Oversampling::X16,
		.humidityOversampling = Bme280Oversampling::Off,
		.filter = Bme280Filter::Off,
		.standbyTime = Bme280StandbyTime::Ms0_5			
	};
}

//
Bme280TwoWire	baro;
SineGenerator	beep;
FT6206			touch;

Variometer      vario;
LocationParser  locParser;
VarioSentence   varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE);
Beeper          beeper;

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
VarioFilter_HarInAirKF2     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
VarioFilter_HarInAirKF4d    varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
VarioFilter_RobinKF         varioFilter;
#endif 

BLEVario        bleDevice;

app_conf_t*     app_conf = NULL;



//
//
//

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



//
//
//

void setup() {
	// initialize pin-mode
	pinMode(POWER_ON, OUTPUT);
	pinMode(BTN_SEL, INPUT);
	pinMode(BTN_LEFT, INPUT);
	pinMode(BTN_RIGHT, INPUT);
	pinMode(BTN_UP, INPUT);	
	pinMode(BTN_DOWN, INPUT);

    digitalWrite(POWER_ON, HIGH);

    //
    app_conf = app_get_conf();
    app_config_init(app_conf);    

    //
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
    bleDevice.begin();

	// setup-wire
	Wire.begin(I2C0_SDA, I2C0_SCL);

	// setup-pwm
	beep.begin(SineGenerator::USE_CHANNEL_1);
	
	// setup-barometer
	baro.begin(Bme280TwoWireAddress::Primary);
	baro.setSettings(varioSettings());		
	
	// setup-touch
	touch.begin();     

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
    #define KF_ACCEL_VARIANCE_DEFAULT   100     // 50 ~ 150
    #define KF_ADAPT_DEFAULT            100     // 50 ~ 150

    varioFilter.begin(1000.0f * KF_ACCEL_VARIANCE_DEFAULT, KF_ADAPT_DEFAULT / 100.0f, 0, 0, 0);
    #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
    varioFilter.Configure(30.0f, 4.0f, altitude);
    #endif    

    vario.begin(CreateBarometer(), &varioFilter);
    locParser.begin(CreateLocationDataSource());
    beeper.begin(CreateTonePlayer());    
}

void loop() {
    // forward serial to bluetooth and reverse : debug purpose only
    #if 1
    while (Serial.available())
    {
        int ch = Serial.read();
        bleDevice.write(ch);
    }

    while (bleDevice.available())
    {
        int ch = bleDevice.read();
        Serial.write(ch);
    }
    #endif

    // button processing
    for (size_t i = 0; i < sizeof(_m1) / sizeof(_m1[0]); i++)
    {
        key_map* kmap = &_m1[i];
        int value = digitalRead(kmap->pin);
        uint8_t state = 0; // release
        if ((kmap->active == 0 && value == 0) || (kmap->active != 0 && value != 0))
            state = 1; // press

        if (kmap->state != state)
        {
            if (state)
            {
                bleDevice.press(kmap->key);
                Serial.printf("Press: %02X\n", kmap->key);
            }
            else
            {
                bleDevice.release(kmap->key);
                Serial.printf("Release: %02X\n", kmap->key);
            }

            kmap->state = state;
        }
    }

    #if 1
    // update each state
    locParser.update();
    int varioUpdated = vario.update();
    beeper.update();

    // location is fixed
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

    // vario-state updated
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

        //
        if (varioNmea.checkInterval())
            varioNmea.begin(app_conf->altitudeAGL, app_conf->speedGround, app_conf->temperature, 0.0f);
    }

    // vario-sentense available?
    static int bt_lock_state = 0; // 0: unlocked, 1: locked_by_vario, 2: locked_by_gps
    if ((bt_lock_state == 0 && varioNmea.available()) || (bt_lock_state == 1))
    {
        bt_lock_state = 1;

        while (varioNmea.available())
        {
            int ch = varioNmea.read();
            if (ch < 0)
                break;

            bleDevice.writeDelayed(ch);
            Serial.write(ch);

            if (ch == '\n')
                bt_lock_state = 0;
        }

        bleDevice.flush();
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

            bleDevice.writeDelayed(ch);
            Serial.write(ch);

            if (ch == '\n')
                bt_lock_state = 0;                
        }

        bleDevice.flush();
    }    
    #endif
}
