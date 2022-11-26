// Application.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#else
#endif
#include <sys/time.h>
#include "device_defines.h"
#include "bsp.h"
#include "utils.h"
#include "ble_vario.h"
#include "timezone.h"

#include "Application.h"

#include "startupWindow.h"
#include "flightWindow.h"


#define THRESHOLD_CIRCLING_HEADING				(6)

#define MAX_GLIDING_COUNT						(10)
#define MIN_GLIDING_COUNT						(0)
#define GLIDING_START_COUNT						(6)
#define GLIDING_EXIT_COUNT						(6)
#define CIRCLING_EXIT_COUNT						(3)


////////////////////////////////////////////////////////////////////////////////////////
//

enum _DeviceMode
{
    DEVICE_MODE_UNDEF,
    DEVICE_MODE_VARIO,
    DEVICE_MODE_VARIO_AND_GPS,
};



//////////////////////////////////////////////      //////////////////////////////////////////
//

static Tone melodyStart[] =
{
    { NOTE_C5, 400, 500 },
    { NOTE_E5, 400, 500 },
    { NOTE_G5, 400, 500 },
};


////////////////////////////////////////////////////////////////////////////////////////
// class Application implementation

Application::Application() 
    : context(nullptr)
    , mode(MODE_INIT)
    , varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE)
    , keyPad(this)
    #if 0
    , bt_lock_state(0)
    #endif
    , gpsFixed(false)
    , dispNeedUpdate(false)
{
    // ...
}

void Application::begin()
{
    // pre-initialize
    // gpio
    // serial
    // wire
    // beep
    // external-devices(baro, touch, ...)
    // BT(X)
    // SPIFF
    // ...

    mode = MODE_INIT;

    // start-vario
    //
    // load-screen
    // battery.begin()
    // update battery power
    // bt.begin()
    // update bt status
    // vario.begin()
    // init device-tick
    
    // loop
    //
    //   keybd.update()
    //   vario.available()
    //     update vario-state (speed, alt, pressure, temp, ...)
    //     update vario-calculator(?)
    //     update beep
    //     update device-tick if device is not quiet
    //     goto deep-sleep if device is quiet
    //     update vario-nmea
    //   nmea.update() && is available
    //     update vario-state (lon, lat, alt, speed, heding, ...)
    //     if it have been fix (READY_FLIGHT)
    //       calibrate vario-altitude & sea-level
    //       update device-time
    //       change device-mode : VARIO_AND_GPS
    //       change flight-mode : READY
    //     if device-mode is VARIO_AND_GPS
    //       check start-flight : whther flight_mode == READ and speed over threshold
    //          start-flight 
    //          ----------------------
    //          auto-turn-on-sound?
    //          play start-flight melody
    //          start logging (if logging is enabled)
    //          reset flight-state & stats
    //          initialize flight-state (takeoff-time, position, ....)
    //          change flight-mode: FLYING
    //          reset flight-tick
    //       update flight-state
    //          flight-time, bearing, distance, altitude-stat
    //          track history
    //          gliding
    //          flight-mode (flying, circling, gliding)
    //       check stop-flight : threashold-(speed, timeout)   
    //   update gps-status
    //   update  logger/bt/battery, ...


    //
    context = DeviceRepository::instance().getContext();

    //
    Screen* screen = Screen::instance();
    screen->setApplication(this);
    screen->activateWindow(new StartupWindow);

    //    
    setTimeZone(context->deviceDefault.timezone * -3600, 0);

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
    speedCalculator.begin(1000, 25);

    if (context->deviceDefault.enableBT)
        bt.begin(0x03, context->deviceDefault.btName);

    beeper.playMelody(melodyStart, sizeof(melodyStart) / sizeof(melodyStart[0]));

    //
    tick_updateTime = millis();
    tick_updateDisp = millis() - 1000;

    tick_stopBase = tick_silentBase = millis();
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

    if (mode == MODE_INIT)
    {
        return;
    }

    if (mode == MODE_SETTING)
    {
        return;
    }

    // update screen in every 500ms if it's need to update
    uint32_t tick = millis();
    if (dispNeedUpdate && tick - tick_updateDisp > 500)
    {
        Window* active = Screen::instance()->peekWindow();
        if (active)
            active->update();

        tick_updateDisp = tick;
        dispNeedUpdate = false;
    }

    //
    locParser.update();
    int varioUpdated = vario.update();
    beeper.update();
    keyPad.update();    // button processing

    if (battery.update())
        context->deviceState.batteryPower = battery.getVoltage();


    //
    if (locParser.availableLocation())
    {
        LOGd("[GPS] %f,%f %f", locParser.getLongitude(), locParser.getLatitude(), locParser.getAltitude());

        // update vario-state
        context->varioState.latitudeLast = context->varioState.latitude;
        context->varioState.longitudeLast = context->varioState.longitude;
        context->varioState.headingLast = context->varioState.heading;

        context->varioState.latitude = locParser.getLatitude();
        context->varioState.longitude = locParser.getLongitude();
        context->varioState.altitudeGPS = locParser.getAltitude();
        context->varioState.speedGround = locParser.getSpeed();
        context->varioState.heading = locParser.getHeading();
        context->varioState.timeCurrent = locParser.getDateTime();
        context->varioState.altitudeGround = agl.getGroundLevel(context->varioState.latitude, context->varioState.longitude);
        context->varioState.altitudeAGL = context->varioState.altitudeGPS - context->varioState.altitudeGround;
        context->varioState.altitudeRef1 = context->varioState.altitudeGPS - context->varioSettings.altitudeRef1;
        context->varioState.altitudeRef2 = context->varioState.altitudeGPS - context->varioSettings.altitudeRef2;
        context->varioState.altitudeRef3 = context->varioState.altitudeGPS - context->varioSettings.altitudeRef3;

        locParser.resetLocation();

        // GPS has been fixed
        if (!gpsFixed)
        {
            LOGv("GPS Fixed!!");

            // update device-time
            setDeviceTime(context->varioState.timeCurrent);

            // why don't you calibarate a bit latter. :  wait for GPS to stabilize.
            //vario.calibrateAltitude(context->varioState.altitudeGPS);
            //vario.calibrateSeaLevel(context->varioState.altitudeGPS);
            // calibrate after 60 seconds
            lv_timer_create(onCalibrateAltitude, 60 * 1000, this);

            // gps-fixed melody
            //beeper.playMelody(toneFixed, sizeof(toneFixed) / sizeof(toneFixed[0]));
        }

        if (mode == MODE_GROUND)
        {
            if (context->varioState.speedGround > context->logger.takeoffSpeed)
                startFlight();
        }
        else
        {
            updateFlightState();

            if (context->varioState.speedGround < context->logger.landingSpeed) // FLIGHT_START_MIN_SPEED
            {
                if ((millis() - tick_stopBase) > context->logger.landingTimeout) // FLIGHT_LANDING_THRESHOLD
                    stopFlight();
            }
            else
            {
                tick_stopBase = millis();
            }
        }

        #if 0
        // update deivce-mode
        if (deviceMode == DEVICE_MODE_VARIO)
        {
            // ready-flight
            // why don't you calibarate a bit latter. :  wait for GPS to stabilize.
            vario.calibrateAltitude(context->varioState.altitudeGPS);
            vario.calibrateSeaLevel(context->varioState.altitudeGPS);

            //struct timeval now = { context->varioState.timeCurrent, 0 };
            //settimeofday(&now, NULL);
            // or
            //setDeviceTime(context->varioState.timeCurrent);

            // play GPS fixed melody
            //beeper.setMelody(...);

            deviceMode = DEVICE_MODE_VARIO_AND_GPS;
            context->flightState.flightMode = FMODE_READY;
        }
        #endif

        dispNeedUpdate = true;
    }

    // update GPS fixed-state
    gpsFixed = locParser.isFixed();

    // vario updated
    if (varioUpdated > 0)
    {
        //
        context->varioState.altitudeBaro = vario.getAltitudeFiltered();
        context->varioState.altitudeCalibrated = vario.getAltitudeCalibrated();
        context->varioState.pressure = vario.getPressure();
        context->varioState.temperature = vario.getTemperature();
        context->varioState.speedVertActive = vario.getVelocity();

        if (speedCalculator.add(context->varioState.speedVertActive) > 0)
        {
            context->varioState.speedVertLazy = speedCalculator.get();
            dispNeedUpdate = true;

            DeviceRepository::instance().updateVSpeedHistory(context->varioState.speedVertLazy);
        }

        /*if (!ble_isConnected())*/
            beeper.setVelocity(context->varioState.speedVertActive);

        //
        if (bt.isConnected() && varioNmea.checkInterval())
        {
            float height = locParser.isFixed() ? context->varioState.altitudeGPS : -1;
            float velocity = context->varioState.speedVertLazy;
            float temperature = context->varioState.temperature;
            float pressure = context->varioState.pressure; // to hPa
            float voltage = battery.getVoltage();

            varioNmea.begin(height, velocity, temperature, pressure, voltage);
        }

        if (mode == MODE_GROUND)
        {
            //if (fabs(context->varioState.speedVertLazy) > context->logger.takeoffVertSpeed)
            //    startFlight();
        }
        else if (!gpsFixed) // case of MODE_FLIYING & UNFIXED-GPS
        {
            if (context->varioState.speedVertLazy < STABLE_SINKING_THRESHOLD || STABLE_SINKING_THRESHOLD < context->varioState.speedVertLazy)
                tick_silentBase = millis();

            if (context->threshold.autoShutdownVario && ((millis() - tick_silentBase) > context->threshold.autoShutdownVario))
                stopFlight();
        }        
    }


    // vario-sentense available?    
    bt.update(varioNmea, locParser);

    // write igc-sentence
    if (igc.isLogging() && locParser.availableIGC()) 
    {
        float altitude = vario.getAltitude();
        igc.updateBaroAltitude(altitude);

        while (locParser.availableIGC())
            igc.write(locParser.readIGC());
    }
}

void Application::onPressed(uint8_t key) 
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

void Application::onLongPressed(uint8_t key) 
{
    LOGv("Key long-pressed: %02X", key);
    if (key == KEY_ENTER)
    {
        LOGi("Turn-off Variometer!!");
        Screen::instance()->showPowerOff();

        bsp_power_on(false);
    }
    else
    {
        Window* active = Screen::instance()->peekWindow();
        if (active)
            active->onLongKeyDown(key);
    }
}

void Application::onReleased(uint8_t key) 
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

#if 0
void Application::update_time()
{
    uint32_t tick = millis();
    if (tick_updateTime - tick > 1000)
    {
        //
        bool fixed = locParser.isFixed();
        if ((!gpsFixed && fixed) || (gpsFixed && fixed))
        {
            gpsFixed = fixed;
            if (gpsFixed)
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

        tick_updateTime = tick;
    }
}
#endif


void Application::updateFlightState()
{
	// update flight time
	context->flightState.flightTime = context->varioState.timeCurrent - context->flightState.takeOffTime;
	// update bearing & distance from takeoff
	context->flightState.bearingTakeoff = GET_BEARING(context->varioState.latitude, context->varioState.longitude, 
			context->flightState.takeOffPos.lat, context->flightState.takeOffPos.lon);
	context->flightState.distTakeoff = GET_DISTANCE(context->varioState.latitude, context->varioState.longitude, 
			context->flightState.takeOffPos.lat, context->flightState.takeOffPos.lon);
	// and update total flight distance
	context->flightState.distFlight += GET_DISTANCE(context->varioState.latitude, context->varioState.longitude, 
			context->varioState.latitudeLast, context->varioState.longitudeLast);
	// add new track point & calculate relative distance
	DeviceRepository::instance().updateTrackHistory(context->varioState.latitude, context->varioState.longitude, context->varioState.speedVertLazy);

	// update flight statistics
	context->flightStats.altitudeMax = _MAX(context->flightStats.altitudeMax, context->varioState.altitudeGPS);
	context->flightStats.altitudeMin = _MIN(context->flightStats.altitudeMin, context->varioState.altitudeGPS);

	context->flightStats.varioMax = _MAX(context->flightStats.varioMax, context->varioState.speedVertLazy);
	context->flightStats.varioMin = _MIN(context->flightStats.varioMin, context->varioState.speedVertLazy);
	

	// check circling
	int16_t deltaHeading = context->varioState.heading - context->varioState.headingLast;

	if (deltaHeading > 180)
		deltaHeading = deltaHeading - 360;
	if (deltaHeading < -180)
		deltaHeading = deltaHeading + 360;

	//context->flightState.deltaHeading_AVG += (int16_t)((deltaHeading - context->flightState.deltaHeading_AVG) * DAMPING_FACTOR_DELTA_HEADING);
	context->flightState.deltaHeading_AVG = deltaHeading;
	context->flightState.deltaHeading_SUM += context->flightState.deltaHeading_AVG;
	context->flightState.deltaHeading_SUM = _CLAMP(context->flightState.deltaHeading_SUM, -450, 450); // one and a half turns

	if (abs(context->flightState.deltaHeading_AVG) < THRESHOLD_CIRCLING_HEADING)	
		context->flightState.glidingCount = _MIN(MAX_GLIDING_COUNT, context->flightState.glidingCount + 1);
	else
		context->flightState.glidingCount = _MAX(MIN_GLIDING_COUNT, context->flightState.glidingCount - 1);

	switch (mode)
	{
	case MODE_FLYING :
		if (abs(context->flightState.deltaHeading_SUM) > 360)
		{
			startCircling();
		}
		else if (context->flightState.glidingCount > GLIDING_START_COUNT)
		{
			startGliding();
		}
		break;

	case MODE_CIRCLING :
		// update time & gain
		context->flightState.circlingTime = context->varioState.timeCurrent - context->flightState.circlingStartTime;
		context->flightState.circlingGain = context->varioState.altitudeGPS - context->flightState.circlingStartPos.alt;

		// checking exit
		if (context->flightState.glidingCount > CIRCLING_EXIT_COUNT)
		{
			// CIRCLING --> FLYING(NORMAL)
			stopCircling();
		}
		break;

	case MODE_GLIDING :
		// update gliding ratio(L/D)
		{
			float dist = GET_DISTANCE(context->flightState.glidingStartPos.lat, context->flightState.glidingStartPos.lon,
				context->varioState.latitude, context->varioState.longitude);

			if (dist > 100)
			{
				if (context->flightState.glidingStartPos.alt > context->varioState.altitudeGPS)
				{
					float diff = context->flightState.glidingStartPos.alt - context->varioState.altitudeGPS;
					context->flightState.glideRatio = dist / diff;
				}
				else
				{
					context->flightState.glideRatio = 0; // INF
				}
			}
		}

		// checking exit
		if (context->flightState.glidingCount < GLIDING_EXIT_COUNT)
		{
			// GLIDING --> FLYING(NORMAL)
			stopGliding();
		}
		break;
	}
}


void Application::startCircling()
{
	// start of circling
	//context->flightState.flightMode = FMODE_CIRCLING;
    mode = MODE_CIRCLING;

	// save circling state
	context->flightState.circlingStartTime = context->varioState.timeCurrent;
	context->flightState.circlingStartPos.lon = context->varioState.longitude;
	context->flightState.circlingStartPos.lat = context->varioState.latitude;
	context->flightState.circlingStartPos.alt = context->varioState.altitudeGPS;
	context->flightState.circlingIncline = -1;

	context->flightState.circlingTime = 0;
	context->flightState.circlingGain = 0;

	context->flightState.glidingCount = 0;

	//
	//display.attachScreen(scrnMan.getCirclingScreen());
	//scrnMan.showCirclingScreen(display);
}

void Application::startGliding()
{
	// start of circling
	//context->flightState.flightMode = FMODE_GLIDING;
    mode = MODE_GLIDING;

	//
	context->flightState.glidingStartPos.lon = context->varioState.longitude;
	context->flightState.glidingStartPos.lat = context->varioState.latitude;
	context->flightState.glidingStartPos.alt = context->varioState.altitudeGPS;

	context->flightState.glideRatio = -1;
}

void Application::stopCircling()
{
	// now normal flying
	//context->flightState.flightMode = FMODE_FLYING;
    mode = MODE_FLYING;

	// update flight-statistics : thermaling count, max-gain
	if (context->flightState.circlingGain > 10 && context->flightState.circlingTime > 0)
	{
		context->flightStats.totalThermaling += 1;
		context->flightStats.thermalingMaxGain = _MAX(context->flightStats.thermalingMaxGain, context->flightState.circlingGain);
	}

	context->flightState.deltaHeading_AVG = 0;
	context->flightState.deltaHeading_SUM = 0;

	//
	//display.attachScreen(scrnMan.getActiveScreen());
	//scrnMan.showActiveScreen(display);
}

void Application::stopGliding()
{
	// now normal flying
	//context->flightState.flightMode = FMODE_FLYING;
    mode = MODE_FLYING;

	//
	context->flightState.glideRatio = -1;
}

void Application::startFlight()
{
    // start-flight
    // turn on sound if auto-turn-on is setted
    if (context->volume.autoTurnOn)
        context->volume.effect = context->volume.vario = 100;

    // play take-off melody
    //beeper.setMelody(...);

    // start bt-logging
    if (context->deviceDefault.enableNmeaLogging)
        bt.startLogging(context->varioState.timeCurrent);

    // start igc-logging & update device-state
    if (context->logger.enable && igc.begin(context->varioState.timeCurrent))
        context->deviceState.statusSDCard = 2;

    // set altitude reference-1
    context->varioSettings.altitudeRef1 = context->varioState.altitudeGPS;

    // reset flight-state/stats
    DeviceRepository::instance().resetFlightState();
    DeviceRepository::instance().resetFlightStats();

    // init flight-state
    context->flightState.takeOffTime = context->varioState.timeCurrent;
    context->flightState.takeOffPos.lat = context->varioState.latitude;
    context->flightState.takeOffPos.lon = context->varioState.longitude;
    context->flightState.bearingTakeoff = -1;
    //context->flightState.flightMode = FMODE_FLYING;
    context->flightState.frontPoint = context->flightState.rearPoint = 0;
    // init flight-stats
    context->flightStats.altitudeMax = context->varioState.altitudeGPS;
    context->flightStats.altitudeMin = context->varioState.altitudeGPS;

    mode = MODE_FLYING;
    tick_stopBase = millis();

    // show take-off notify-message
    // Screen->topWindow->showNotify("take-off");
    LOGv("Application::startFlight()");
}

void Application::stopFlight()
{
    // stop-flight
    mode = MODE_GROUND;
    //context->flightState.flightMode = FMODE_READY;
    context->flightState.bearingTakeoff = -1;

    // play landing-melody
    // ...

    // stop nmea-logging
    bt.stopLogging();

    // stop igc-logging
    if (igc.isLogging())
    {
        igc.end(context->varioState.timeCurrent);
        context->deviceState.statusSDCard = 1;
    }

    // show lading-message
    // screen->showNotify("landing");
    LOGv("Application::stopFlight()");

    if (context->volume.autoTurnOn)
    {
        context->volume.vario = context->volume.varioDefault;
        context->volume.effect = context->volume.effectDefault;
    }    
}

void Application::startVario()
{
    context->deviceState.statusSDCard = 0;
    context->deviceState.batteryPower = battery.getVoltage();

    //if (context->deviceDefault.enableBT)
    //    bt.begin(3);
    //
    //vario.begin();

    mode = MODE_GROUND;
    tick_silentBase = millis();
    tick_updateDisp = millis() - 1000;
    dispNeedUpdate = true;

    LOGv("Application::startVario()");
}

void Application::onCalibrateAltitude(struct _lv_timer_t * timer)
{
    Application* app = (Application *)timer->user_data;
    app->calibrateAltitude();

    lv_timer_del(timer);
}

void Application::calibrateAltitude()
{
    LOGv("Application::calibrateAltitude()");

    vario.calibrateAltitude(context->varioState.altitudeGPS);
    vario.calibrateSeaLevel(context->varioState.altitudeGPS);
}
