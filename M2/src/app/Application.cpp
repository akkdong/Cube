// Application.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <sys/time.h>
#include "board.h"
#include "bsp.h"
#include "utils.h"
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
    : contextPtr(nullptr)
    , mode(MODE_INIT)
    , varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE)
    , keyPad(this)
    #if 0
    , bt_lock_state(0)
    #endif
    , gpsFixed(false)
//  , dispNeedUpdate(false)
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
    contextPtr = DeviceRepository::instance().getContext();

    //
    screenPtr = Screen::instance();

    screenPtr->setApplication(this);
    screenPtr->activateWindow(new StartupWindow);

    //    
    setTimeZone(contextPtr->deviceDefault.timezone * -3600, 0);

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
    battery.begin();

    if (contextPtr->deviceDefault.enableBT)
        bt.begin(0x03, contextPtr->deviceDefault.btName);

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
    #if 1
    uint32_t tick = millis();
    if (dispNeedUpdate && tick - tick_updateDisp > 500)
    {
        Window* active = Screen::instance()->peekWindow();
        if (active)
            active->update();

        tick_updateDisp = tick;
        dispNeedUpdate = false;
        //LOGi("update: %u", millis() - tick);
    }
    #endif

    //
    locParser.update();
    int varioUpdated = vario.update();
    beeper.update();
    keyPad.update();    // button processing

    if (battery.update())
        contextPtr->deviceState.batteryPower = battery.getVoltage();


    //
    if (locParser.availableLocation())
    {
        LOGd("[GPS] %f,%f %f", locParser.getLongitude(), locParser.getLatitude(), locParser.getAltitude());

        // update vario-state
        contextPtr->varioState.latitudeLast = contextPtr->varioState.latitude;
        contextPtr->varioState.longitudeLast = contextPtr->varioState.longitude;
        contextPtr->varioState.headingLast = contextPtr->varioState.heading;

        contextPtr->varioState.latitude = locParser.getLatitude();
        contextPtr->varioState.longitude = locParser.getLongitude();
        contextPtr->varioState.altitudeGPS = locParser.getAltitude();
        contextPtr->varioState.speedGround = locParser.getSpeed();
        contextPtr->varioState.heading = locParser.getHeading();
        contextPtr->varioState.timeCurrent = locParser.getDateTime();
        contextPtr->varioState.altitudeGround = agl.getGroundLevel(contextPtr->varioState.latitude, contextPtr->varioState.longitude);
        contextPtr->varioState.altitudeAGL = contextPtr->varioState.altitudeGPS - contextPtr->varioState.altitudeGround;
        contextPtr->varioState.altitudeRef1 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef1;
        contextPtr->varioState.altitudeRef2 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef2;
        contextPtr->varioState.altitudeRef3 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef3;

        locParser.resetLocation();

        // GPS has been fixed
        if (!gpsFixed)
        {
            LOGv("GPS Fixed!!");

            // update device-time
            setDeviceTime(contextPtr->varioState.timeCurrent);

            // why don't you calibarate a bit latter. :  wait for GPS to stabilize.
            //vario.calibrateAltitude(contextPtr->varioState.altitudeGPS);
            //vario.calibrateSeaLevel(contextPtr->varioState.altitudeGPS);
            // calibrate after 60 seconds
            lv_timer_t* timer = lv_timer_create(onCalibrateAltitude, 60 * 1000, this);
            lv_timer_set_repeat_count(timer, 1);

            // gps-fixed melody
            Screen::instance()->notifyMesage("GPS Fixed!!");
            //beeper.playMelody(toneFixed, sizeof(toneFixed) / sizeof(toneFixed[0]));
        }

        if (mode == MODE_GROUND)
        {
            if (contextPtr->varioState.speedGround > contextPtr->logger.takeoffSpeed)
                startFlight();
        }
        else
        {
            updateFlightState();

            if (contextPtr->varioState.speedGround < contextPtr->logger.landingSpeed) // FLIGHT_START_MIN_SPEED
            {
                if ((millis() - tick_stopBase) > contextPtr->logger.landingTimeout) // FLIGHT_LANDING_THRESHOLD
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
            vario.calibrateAltitude(contextPtr->varioState.altitudeGPS);
            vario.calibrateSeaLevel(contextPtr->varioState.altitudeGPS);

            //struct timeval now = { contextPtr->varioState.timeCurrent, 0 };
            //settimeofday(&now, NULL);
            // or
            //setDeviceTime(contextPtr->varioState.timeCurrent);

            // play GPS fixed melody
            //beeper.setMelody(...);

            deviceMode = DEVICE_MODE_VARIO_AND_GPS;
            contextPtr->flightState.flightMode = FMODE_READY;
        }
        #endif

        dispNeedUpdate = true;
    }

    // update GPS fixed-state
    bool fixed = locParser.isFixed();
    if ((fixed && !gpsFixed) || (!fixed && gpsFixed))
    {
        gpsFixed = fixed;
        contextPtr->deviceState.statusGPS = fixed ? 1 :0;
    }

    // vario updated
    if (varioUpdated > 0)
    {
        //
        vario.resetUpdate();

        //
        contextPtr->varioState.altitudeBaro = vario.getAltitudeFiltered();
        contextPtr->varioState.altitudeCalibrated = vario.getAltitudeCalibrated();
        contextPtr->varioState.pressure = vario.getPressure();
        contextPtr->varioState.temperature = vario.getTemperature();
        contextPtr->varioState.speedVertActive = vario.getVelocity();
        //static uint32_t lastTick = millis();
        //uint32_t curTick = millis();
        //LOGi("%u", curTick - lastTick);
        //lastTick = curTick;

        if (speedCalculator.add(contextPtr->varioState.speedVertActive) > 0)
        {
            contextPtr->varioState.speedVertLazy = speedCalculator.get();
            dispNeedUpdate = true;

            DeviceRepository::instance().updateVSpeedHistory(contextPtr->varioState.speedVertLazy);
        }

        /*if (!ble_isConnected())*/
            beeper.setVelocity(contextPtr->varioState.speedVertActive);

        //
        if (bt.isConnected() && varioNmea.checkInterval())
        {
            float height = locParser.isFixed() ? contextPtr->varioState.altitudeGPS : -1;
            float velocity = contextPtr->varioState.speedVertLazy;
            float temperature = contextPtr->varioState.temperature;
            float pressure = contextPtr->varioState.pressure; // to hPa
            float voltage = battery.getVoltage();

            varioNmea.begin(height, velocity, temperature, pressure, voltage);
        }

        if (mode == MODE_GROUND)
        {
            //if (fabs(contextPtr->varioState.speedVertLazy) > contextPtr->logger.takeoffVertSpeed)
            //    startFlight();
        }
        else if (!gpsFixed) // case of MODE_FLIYING & UNFIXED-GPS
        {
            if (contextPtr->varioState.speedVertLazy < STABLE_SINKING_THRESHOLD || STABLE_SINKING_THRESHOLD < contextPtr->varioState.speedVertLazy)
                tick_silentBase = millis();

            if (contextPtr->threshold.autoShutdownVario && ((millis() - tick_silentBase) > contextPtr->threshold.autoShutdownVario))
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
    Window* active = screenPtr->peekWindow();
    if (active)
        active->onKeyDown(key);
    #endif
}

void Application::onLongPressed(uint8_t key) 
{
    LOGv("Key long-pressed: %02X", key);
    /*
    if (key == KEY_ENTER)
    {
        LOGi("Turn-off Variometer!!");
        screenPtr->showPowerOff();

        bsp_power_on(false);
    }
    else
    */
    {
        Window* active = screenPtr->peekWindow();
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
    Window* active = screenPtr->peekWindow();
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
	contextPtr->flightState.flightTime = contextPtr->varioState.timeCurrent - contextPtr->flightState.takeOffTime;
	// update bearing & distance from takeoff
	contextPtr->flightState.bearingTakeoff = GET_BEARING(contextPtr->varioState.latitude, contextPtr->varioState.longitude, 
			contextPtr->flightState.takeOffPos.lat, contextPtr->flightState.takeOffPos.lon);
	contextPtr->flightState.distTakeoff = GET_DISTANCE(contextPtr->varioState.latitude, contextPtr->varioState.longitude, 
			contextPtr->flightState.takeOffPos.lat, contextPtr->flightState.takeOffPos.lon);
	// and update total flight distance
	contextPtr->flightState.distFlight += GET_DISTANCE(contextPtr->varioState.latitude, contextPtr->varioState.longitude, 
			contextPtr->varioState.latitudeLast, contextPtr->varioState.longitudeLast);
	// add new track point & calculate relative distance
	DeviceRepository::instance().updateTrackHistory(contextPtr->varioState.latitude, contextPtr->varioState.longitude, contextPtr->varioState.speedVertLazy);

	// update flight statistics
	contextPtr->flightStats.altitudeMax = _MAX(contextPtr->flightStats.altitudeMax, contextPtr->varioState.altitudeGPS);
	contextPtr->flightStats.altitudeMin = _MIN(contextPtr->flightStats.altitudeMin, contextPtr->varioState.altitudeGPS);

	contextPtr->flightStats.varioMax = _MAX(contextPtr->flightStats.varioMax, contextPtr->varioState.speedVertLazy);
	contextPtr->flightStats.varioMin = _MIN(contextPtr->flightStats.varioMin, contextPtr->varioState.speedVertLazy);
	

	// check circling
	int16_t deltaHeading = contextPtr->varioState.heading - contextPtr->varioState.headingLast;

	if (deltaHeading > 180)
		deltaHeading = deltaHeading - 360;
	if (deltaHeading < -180)
		deltaHeading = deltaHeading + 360;

	//contextPtr->flightState.deltaHeading_AVG += (int16_t)((deltaHeading - contextPtr->flightState.deltaHeading_AVG) * DAMPING_FACTOR_DELTA_HEADING);
	contextPtr->flightState.deltaHeading_AVG = deltaHeading;
	contextPtr->flightState.deltaHeading_SUM += contextPtr->flightState.deltaHeading_AVG;
	contextPtr->flightState.deltaHeading_SUM = _CLAMP(contextPtr->flightState.deltaHeading_SUM, -450, 450); // one and a half turns

	if (abs(contextPtr->flightState.deltaHeading_AVG) < THRESHOLD_CIRCLING_HEADING)	
		contextPtr->flightState.glidingCount = _MIN(MAX_GLIDING_COUNT, contextPtr->flightState.glidingCount + 1);
	else
		contextPtr->flightState.glidingCount = _MAX(MIN_GLIDING_COUNT, contextPtr->flightState.glidingCount - 1);

	switch (mode)
	{
	case MODE_FLYING :
		if (abs(contextPtr->flightState.deltaHeading_SUM) > 360)
		{
			startCircling();
		}
		else if (contextPtr->flightState.glidingCount > GLIDING_START_COUNT)
		{
			startGliding();
		}
		break;

	case MODE_CIRCLING :
		// update time & gain
		contextPtr->flightState.circlingTime = contextPtr->varioState.timeCurrent - contextPtr->flightState.circlingStartTime;
		contextPtr->flightState.circlingGain = contextPtr->varioState.altitudeGPS - contextPtr->flightState.circlingStartPos.alt;

		// checking exit
		if (contextPtr->flightState.glidingCount > CIRCLING_EXIT_COUNT)
		{
			// CIRCLING --> FLYING(NORMAL)
			stopCircling();
		}
		break;

	case MODE_GLIDING :
		// update gliding ratio(L/D)
		{
			float dist = GET_DISTANCE(contextPtr->flightState.glidingStartPos.lat, contextPtr->flightState.glidingStartPos.lon,
				contextPtr->varioState.latitude, contextPtr->varioState.longitude);

			if (dist > 100)
			{
				if (contextPtr->flightState.glidingStartPos.alt > contextPtr->varioState.altitudeGPS)
				{
					float diff = contextPtr->flightState.glidingStartPos.alt - contextPtr->varioState.altitudeGPS;
					contextPtr->flightState.glideRatio = dist / diff;
				}
				else
				{
					contextPtr->flightState.glideRatio = 0; // INF
				}
			}
		}

		// checking exit
		if (contextPtr->flightState.glidingCount < GLIDING_EXIT_COUNT)
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
	//contextPtr->flightState.flightMode = FMODE_CIRCLING;
    mode = MODE_CIRCLING;

	// save circling state
	contextPtr->flightState.circlingStartTime = contextPtr->varioState.timeCurrent;
	contextPtr->flightState.circlingStartPos.lon = contextPtr->varioState.longitude;
	contextPtr->flightState.circlingStartPos.lat = contextPtr->varioState.latitude;
	contextPtr->flightState.circlingStartPos.alt = contextPtr->varioState.altitudeGPS;
	contextPtr->flightState.circlingIncline = -1;

	contextPtr->flightState.circlingTime = 0;
	contextPtr->flightState.circlingGain = 0;

	contextPtr->flightState.glidingCount = 0;

	//
	//display.attachScreen(scrnMan.getCirclingScreen());
	//scrnMan.showCirclingScreen(display);
}

void Application::startGliding()
{
	// start of circling
	//contextPtr->flightState.flightMode = FMODE_GLIDING;
    mode = MODE_GLIDING;

	//
	contextPtr->flightState.glidingStartPos.lon = contextPtr->varioState.longitude;
	contextPtr->flightState.glidingStartPos.lat = contextPtr->varioState.latitude;
	contextPtr->flightState.glidingStartPos.alt = contextPtr->varioState.altitudeGPS;

	contextPtr->flightState.glideRatio = -1;
}

void Application::stopCircling()
{
	// now normal flying
	//contextPtr->flightState.flightMode = FMODE_FLYING;
    mode = MODE_FLYING;

	// update flight-statistics : thermaling count, max-gain
	if (contextPtr->flightState.circlingGain > 10 && contextPtr->flightState.circlingTime > 0)
	{
		contextPtr->flightStats.totalThermaling += 1;
		contextPtr->flightStats.thermalingMaxGain = _MAX(contextPtr->flightStats.thermalingMaxGain, contextPtr->flightState.circlingGain);
	}

	contextPtr->flightState.deltaHeading_AVG = 0;
	contextPtr->flightState.deltaHeading_SUM = 0;

	//
	//display.attachScreen(scrnMan.getActiveScreen());
	//scrnMan.showActiveScreen(display);
}

void Application::stopGliding()
{
	// now normal flying
	//contextPtr->flightState.flightMode = FMODE_FLYING;
    mode = MODE_FLYING;

	//
	contextPtr->flightState.glideRatio = -1;
}

void Application::startFlight()
{
    // start-flight
    // turn on sound if auto-turn-on is setted
    if (contextPtr->volume.autoTurnOn)
        contextPtr->volume.effect = contextPtr->volume.vario = 100;

    // play take-off melody
    //beeper.setMelody(...);

    // start bt-logging
    if (contextPtr->deviceDefault.enableNmeaLogging)
        bt.startLogging(contextPtr->varioState.timeCurrent);

    // start igc-logging & update device-state
    if (contextPtr->logger.enable && igc.begin(contextPtr->varioState.timeCurrent))
        contextPtr->deviceState.statusSDCard = 2;

    // set altitude reference-1
    contextPtr->varioSettings.altitudeRef1 = contextPtr->varioState.altitudeGPS;

    // reset flight-state/stats
    DeviceRepository::instance().resetFlightState();
    DeviceRepository::instance().resetFlightStats();

    // init flight-state
    contextPtr->flightState.takeOffTime = contextPtr->varioState.timeCurrent;
    contextPtr->flightState.takeOffPos.lat = contextPtr->varioState.latitude;
    contextPtr->flightState.takeOffPos.lon = contextPtr->varioState.longitude;
    contextPtr->flightState.bearingTakeoff = -1;
    //contextPtr->flightState.flightMode = FMODE_FLYING;
    contextPtr->flightState.frontPoint = contextPtr->flightState.rearPoint = 0;
    // init flight-stats
    contextPtr->flightStats.altitudeMax = contextPtr->varioState.altitudeGPS;
    contextPtr->flightStats.altitudeMin = contextPtr->varioState.altitudeGPS;

    mode = MODE_FLYING;
    tick_stopBase = millis();

    // show take-off notify-message
    Screen::instance()->notifyMesage("Take off!!");
    LOGv("Application::startFlight()");
}

void Application::stopFlight()
{
    // stop-flight
    mode = MODE_GROUND;
    //contextPtr->flightState.flightMode = FMODE_READY;
    contextPtr->flightState.bearingTakeoff = -1;

    // play landing-melody
    // ...

    // stop nmea-logging
    bt.stopLogging();

    // stop igc-logging
    if (igc.isLogging())
    {
        igc.end(contextPtr->varioState.timeCurrent);
        contextPtr->deviceState.statusSDCard = 1;
    }

    // show lading-message
    Screen::instance()->notifyMesage("Landing...");
    LOGv("Application::stopFlight()");

    if (contextPtr->volume.autoTurnOn)
    {
        contextPtr->volume.vario = contextPtr->volume.varioDefault;
        contextPtr->volume.effect = contextPtr->volume.effectDefault;
    }    
}

void Application::startVario()
{
    contextPtr->deviceState.statusSDCard = SD_CARD.valid() ? 1 : 0;
    contextPtr->deviceState.statusGPS = 0;
    contextPtr->deviceState.batteryPower = battery.getVoltage();

    //if (contextPtr->deviceDefault.enableBT)
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
    Screen::instance()->notifyMesage("Calibrate Altitude...");

    #if !USE_SEALEVEL_CALIBRATION
    vario.calibrateAltitude(contextPtr->varioState.altitudeGPS);
    #else
    vario.calibrateSeaLevel(contextPtr->varioState.altitudeGPS);
    #endif
}
