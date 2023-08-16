// Application.cpp
//

#include <Arduino.h>
#include "board.h"
#include "config.h"
#include "logger.h"
#include "timezone.h"
#include "utils.h"
#include "Application.h"
#include "Widgets.h"
#include "Window.h"
#include "MainWindow.h"
#include "font/binaryttf.h"
#include "ImageResource.h"
#include "KeypadInput.h"
#include "DeviceRepository.h"

#define THRESHOLD_CIRCLING_HEADING				(6)

#define MAX_GLIDING_COUNT						(10)
#define MIN_GLIDING_COUNT						(0)
#define GLIDING_START_COUNT						(6)
#define GLIDING_EXIT_COUNT						(6)
#define CIRCLING_EXIT_COUNT						(3)


///////////////////////////////////////////////////////////////////////////////////////
// class Application

Application::Application() 
    : KEY(this)
    , DEV(Serial1)
    , DBG(Serial)
    , Display(&EPD)
    , contextPtr(nullptr)
    , deviceMode(MODE_INIT)
    , varioNmea(VARIOMETER_DEFAULT_NMEA_SENTENCE)
{
}

void Application::initBoard()
{
    // GPIOs
    pinMode(KEY_POWER_MAIN, OUTPUT);
    pinMode(KEY_POWER_EPD, OUTPUT);
    pinMode(KEY_POWER_EXT, OUTPUT);
    pinMode(KEY_LEFT, INPUT);
    pinMode(KEY_PUSH, INPUT);
    pinMode(KEY_RIGHT, INPUT);

    // hold main power
    digitalWrite(KEY_POWER_MAIN, 1);
    // turn-on power for peripherals
    digitalWrite(KEY_POWER_EPD, 1);
    digitalWrite(KEY_POWER_EXT, 1);
    delay(1000);

    // Communications
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, UART_DEVICE_RX, UART_DEVICE_TX);
    Wire.begin(TOUCH_SDA, TOUCH_SCL, (uint32_t) 400000U);
    SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS_SD);

    // mount SPIFFS
    SPIFFS.begin();
    // check SD-Card and ready to use
    SD.begin(4, SPI, 20000000);

    if (SD.cardType() != CARD_NONE)
    {
        // do firmware-update
        #if SUPPPORT_FIRMWAREUPDATE
        FirmwareUpdater fu;

		if (fu.checkUpdate())
		{
			//
			fu.performUpdate(display);
			//
			ESP.restart();
		}		
		// else go down
        #endif
    }
    else
    {
        LOGi("No SD card inserted!!");
    }
}

void Application::begin()
{
    //
    EPD.begin(M5EPD_SCK_PIN, M5EPD_MOSI_PIN, M5EPD_MISO_PIN, M5EPD_CS_PIN, M5EPD_BUSY_PIN);
    EPD.Clear(true);
    EPD.SetRotation(M5EPD_Driver::ROTATE_0);

    EPD.WritePartGram4bpp((LCD_WIDTH - 256) / 2, (LCD_HEIGHT - 256) / 2, 256, 256, ImageResource_logo_wolf_large_256x256);
    EPD.UpdateFull(UPDATE_MODE_GC16);

    //
    TP.SetRotation(GT911::ROTATE_0);
    TP.begin(TOUCH_SDA, TOUCH_SCL, TOUCH_INT);
    // battery begin
    BAT.begin();
    //
    KEY.begin(CreateKeypadInput());
    //
    //TH.Begin();
    //
    NMEA.begin();


    // GUI startup
    Display.loadFont(binaryttf, sizeof(binaryttf));
    Display.createCanvas(M5EPD_PANEL_W, M5EPD_PANEL_H);
    Display.createRender(32);
    Display.createRender(48);
    Display.createRender(76);
    Display.createRender(92);

    // activate main-window
    MainWindow* pMainWnd = new MainWindow(&Display);
    Scrn.activateWindow(pMainWnd);


    //
    //
    //
    DeviceRepository& repo = DeviceRepository::instance();
    repo.reset();
    repo.loadPref();

    contextPtr = DeviceRepository::instance().getContext();

    //
    setTimeZone(0 - contextPtr->deviceDefault.timezoneOffset, 0);
    //
    disableCore0WDT();

    //
    {
        int retry = 0;
        int status;
    
        while ((status == BAT.update()) > 0 || retry++ > 10)
            delay(10);

        if (status > 0)
        {
            // update battery voltage
            float volt = BAT.getVoltage();
            LOGv("Battery: %.2fv", volt);

            contextPtr->deviceState.batteryPower = volt;
        }
    }

    //
    msgQueue = xQueueCreate(10, sizeof(Message));

    xTaskCreate(ScreenTask, "SCR", 4096, this, 0, &taskScreen);    
    xTaskCreate(DeviceTask, "DEV", 2048, this, 0, &taskDevice); 


    //
    //
    //
    deviceMode = MODE_INIT;

    if (contextPtr->deviceDefault.enableBT)
        BT.begin(0x03, contextPtr->deviceDefault.btName);    

    //
    uint32_t now = millis();
    tick_stopBase = tick_silentBase = now;
    tick_updateTime = now;
    tick_updateDisp = now - 1000;

    startVario();
}

void Application::update()
{
    // read serial (NEMA)
    // parse nmea
    // if gps-fixed
    //   update device-state
    //   update canvas(screen)
    // if vario-fixed
    //   update device-sate
    //   update canvas(screen)
    // 

    if (prepareShutdown)
    {
        if (deviceMode != MODE_GROUND)
        {
            LOGv("Stop flight if it's not ground");
            stopFlight();
        }

        return;
    }

    bool engMode = false;
    Stream& input = engMode ? Serial : Serial1;
    uint32_t now = millis();

    while (input.available())
    {
        int ch = input.read();
        int type = NMEA.update(ch);

        //
        if (type == 1) // parsed GPS sentence
        {
            bool fixed = NMEA.isFixed();
            if (fixed) // is fixed?
            {
                LOGv("GPS: %f %f %f %f", NMEA.getLatitude(), NMEA.getLongitude(), NMEA.getSpeed(), NMEA.getTrack());

                // LOCK
                contextPtr->varioState.latitudeLast = contextPtr->varioState.latitude;
                contextPtr->varioState.longitudeLast = contextPtr->varioState.longitude;
                contextPtr->varioState.headingLast = contextPtr->varioState.heading;

                contextPtr->varioState.latitude = NMEA.getLatitude();
                contextPtr->varioState.longitude = NMEA.getLongitude();
                contextPtr->varioState.altitudeGPS = NMEA.getAltitude();
                contextPtr->varioState.speedGround = NMEA.getSpeed();
                contextPtr->varioState.heading = NMEA.getTrack();
                contextPtr->varioState.timeCurrent = NMEA.getDateTime() + contextPtr->deviceDefault.timezoneOffset;
                contextPtr->varioState.altitudeGround = AGL.getGroundLevel(contextPtr->varioState.latitude, contextPtr->varioState.longitude);
                contextPtr->varioState.altitudeAGL = contextPtr->varioState.altitudeGPS - contextPtr->varioState.altitudeGround;
                contextPtr->varioState.altitudeRef1 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef1;
                contextPtr->varioState.altitudeRef2 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef2;
                contextPtr->varioState.altitudeRef3 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef3;        
                // UNLOCK

                if (Application::deviceMode == MODE_GROUND)
                {
                    if (contextPtr->varioState.speedGround > contextPtr->logger.takeoffSpeed)
                    {
                        startFlight();
                        sendMessage(MSG_TAKEOFF, 0);
                    }
                }
                else // MODE_FLYING/CIRCLING/GLIDING/SETTING
                {
                    if (IGC.isLogging())
                    {
                        igcSentence.begin(
                            contextPtr->varioState.timeCurrent - contextPtr->deviceDefault.timezoneOffset,
                            contextPtr->varioState.latitude,
                            contextPtr->varioState.longitude,
                            contextPtr->varioState.altitudeBaro,
                            contextPtr->varioState.altitudeGPS);

                        #if LOG_LEVEL >= LOG_LEVEL_VERBOSE
                        igcSentence.dump(DBG);
                        #endif
                    }

                    updateFlightState();
                    checkFlightMode();
                }

                sendMessage(MSG_UPDATE_GPS, 0);
            }

            if ((fixed && !gpsFixed) || (!fixed && gpsFixed))
            {
                gpsFixed = fixed;
                contextPtr->deviceState.statusGPS = fixed ? 1 : 0;
                LOGv("GPS Fixed: %s", gpsFixed ? "TRUE" : "FALSE");

                if (fixed)
                {
                    // update device-time
                    setDeviceTime(contextPtr->varioState.timeCurrent);
                }
                else
                {
                    #if 0
                    contextPtr->varioState.altitudeGPS = 0;
                    contextPtr->varioState.speedGround = 0;
                    contextPtr->varioState.heading = 0;

                    sendMessage(MSG_UPDATE_GPS, 0);
                    #endif
                }

                // notify gps-fixed
                sendMessage(MSG_GPS_FIXED, fixed);
            }
        }
        else if (type == 2) // parsed VARIO sentence
        {
            LOGv("VARIO: %f %f %f %f", NMEA.getPressure(), NMEA.getTemperature(), NMEA.getAltitudeBaro(), NMEA.getVerticalSpeed());

            // LOCK
            contextPtr->varioState.temperature = NMEA.getTemperature();
            contextPtr->varioState.pressure = NMEA.getPressure();
            contextPtr->varioState.altitudeBaro = NMEA.getAltitudeBaro();
            contextPtr->varioState.speedVertActive = NMEA.getVerticalSpeed();

            contextPtr->volume.vario = NMEA.isMuted() ? 0 : 100;
            contextPtr->volume.effect = NMEA.isMuted() ? 0 : 100;

            //contextPtr->varioState.altitudeBaro = calculateAltitude(contextPtr->varioState.pressure);
            //contextPtr->varioState.altitudeCalibrated = contextPtr->varioState.altitudeBaro - contextPtr->varioState.altitudeDrift;
            //contextPtr->varioState.pressureLazy += (contextPtr->varioState.pressure - contextPtr->varioState.pressureLazy) * 1.0;
            //contextPtr->varioState.speedVertLazy += (contextPtr->varioState.speedVertActive - contextPtr->varioState.speedVertLazy) * 1.0;
            contextPtr->varioState.pressureLazy = contextPtr->varioState.pressure;
            contextPtr->varioState.speedVertLazy = contextPtr->varioState.speedVertActive;
            // UNLOCK

            // update VARIO relative widgets
            if (!gpsFixed && (now - tick_updateTime) >= 500)
            {
                sendMessage(MSG_UPDATE_VARIO);
                tick_updateTime = now;
            }

            //if (/* busy? interval? */ varioNmea.checkInterval())
            {
                // update vario-setence
                float height = contextPtr->deviceState.statusGPS == 1 ? contextPtr->varioState.altitudeGPS : -1;
                float velocity = contextPtr->varioState.speedVertLazy;
                float temperature = contextPtr->varioState.temperature;
                float pressure = contextPtr->varioState.pressureLazy; // to hPa
                float voltage = contextPtr->deviceState.batteryPower;

                varioNmea.begin(height, velocity, temperature, pressure, voltage);

                //
                if (Application::deviceMode != MODE_GROUND && contextPtr->deviceState.statusGPS == 0)
                {
                    uint32_t curTick = now; // millis()

                    if (velocity < STABLE_SINKING_THRESHOLD || STABLE_SINKING_THRESHOLD < velocity)
                        tick_silentBase = curTick;

                    if (contextPtr->threshold.autoShutdownVario && ((curTick - tick_silentBase) > contextPtr->threshold.autoShutdownVario))
                    {   
                        stopFlight();                     
                        sendMessage(MSG_LANDING);
                    }
                }
            }
        }
        else if (type == 3) // parsed KBD sentence
        {
            uint8_t key, state;
            key = NMEA.getLastKey(&state);
            uint32_t msg = state == 0 ? MSG_KEY_RELEASED : (state == 1 ? MSG_KEY_PRESSED : MSG_KEY_LONG_PRESSED);
            LOGv("KEY: %d(%s)", key, state == 0 ? "OFF" : (state == 1 ? "ON" : "LONG"));

            switch (key)
            {
            case 0xB0: // ^  UP
                sendMessage(msg, EXT_KEY_UP);
                break;
            case 0xB1: // \/ DN
                sendMessage(msg, EXT_KEY_DOWN);
                break;
            case 0xB2: // <  ESC
                sendMessage(msg, EXT_KEY_LEFT);
                break;
            case 0xB3: // >  ENTER
                sendMessage(msg, EXT_KEY_RIGHT);
                break;
            }          
        }
    }

    // update BT state
    if (contextPtr->deviceState.statusBT > 0)
    {
        bool connected = BT.isConnected();
        if ((connected && contextPtr->deviceState.statusBT < 2) || (!connected && contextPtr->deviceState.statusBT > 1))
            contextPtr->deviceState.statusBT = connected ? 2 : 1;
    }

    // flush bt buffer
    BT.update(varioNmea, NMEA);

    // flush igc-sentence
    if (IGC.isLogging() && igcSentence.available()) 
    {
        //float altitude = NMEA.getAltitude();
        //IGC.updateBaroAltitude(altitude);

        while (igcSentence.available())
            IGC.write(igcSentence.read());
    }

    //delay(1);
}

void Application::startVario()
{
    LOGi("Application::startVario()");

    contextPtr->deviceState.statusSDCard = SD.cardType() != CARD_NONE ? 1 : 0;
    contextPtr->deviceState.statusGPS = 0;
    contextPtr->deviceState.batteryPower = BAT.getVoltage();

    deviceMode = MODE_GROUND;
    tick_silentBase = millis();
    tick_updateDisp = tick_silentBase - 1000;
    gpsFixed = false;
    prepareShutdown = false;

    vTaskResume(taskScreen);
    vTaskResume(taskDevice);
}

void Application::startFlight()
{
    // turn on sound if auto-turn-on is setted
    if (contextPtr->volume.autoTurnOn)
    {
        //contextPtr->volume.effect = contextPtr->volume.vario = 100;
        Serial1.println("MUTE 0");
    }

    // start bt-logging
    if (contextPtr->deviceDefault.enableNmeaLogging)
        BT.startLogging(contextPtr->varioState.timeCurrent);

    // start igc-logging & update device-state
    if (contextPtr->logger.enable && IGC.begin(contextPtr->varioState.timeCurrent))
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

    LOGi("Start Flight!");
    deviceMode = MODE_FLYING;
    tick_stopBase = millis();
}

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
	contextPtr->flightState.distFlight = GET_DISTANCE(contextPtr->varioState.latitude, contextPtr->varioState.longitude, 
			contextPtr->varioState.latitudeLast, contextPtr->varioState.longitudeLast);
    contextPtr->flightState.distFlightAccum += contextPtr->flightState.distFlight;
	// add new track point & calculate relative distance
    #if 1 // OLD METHOD
	DeviceRepository::instance().updateTrackHistory(contextPtr->varioState.latitude, contextPtr->varioState.longitude, contextPtr->varioState.speedVertLazy);
    #else // NEW BUT TRICKY METHOD
    DeviceRepository::instance().updateTrackHistory();
    #endif

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
}

void Application::checkFlightMode()
{
	switch (deviceMode)
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

    if (/*deviceMode != MODE_GROUND &&*/ contextPtr->varioState.speedGround < contextPtr->logger.landingSpeed) // FLIGHT_START_MIN_SPEED
    {
        uint32_t interval = millis() - this->tick_stopBase;
        //LOGv("stop interval: %u", interval);
        if (interval > contextPtr->logger.landingTimeout) // FLIGHT_LANDING_THRESHOLD
        {
            stopFlight();
            sendMessage(MSG_LANDING, 0);
        }
    }
    else
    {
        this->tick_stopBase = millis();
    }
}

void Application::stopFlight()
{
    // stop-flight
    deviceMode = MODE_GROUND;
    //contextPtr->flightState.flightMode = FMODE_READY;
    contextPtr->flightState.bearingTakeoff = -1;

    // stop nmea-logging
    BT.stopLogging();

    // stop igc-logging
    if (IGC.isLogging())
    {
        IGC.end(contextPtr->varioState.timeCurrent);
        contextPtr->deviceState.statusSDCard = 1;
    }

    /*
    LOGv("Application::stopFlight()");

    // show lading-message
    Application::lock.enter();
    Screen::instance()->notifyMesage("Landing...");
    Application::lock.leave();

    // play landing-melody
    beeper.playMelody(melodyLanding, sizeof(melodyLanding) / sizeof(melodyLanding[0]));
    */

    if (contextPtr->volume.autoTurnOn)
    {
        //contextPtr->volume.vario = contextPtr->volume.varioDefault;
        //contextPtr->volume.effect = contextPtr->volume.effectDefault;

        Serial1.println("MUTE 1");
    }

    LOGi("Stop Flight!");
}


void Application::startCircling()
{
	// start of circling
	//contextPtr->flightState.flightMode = FMODE_CIRCLING;
    deviceMode = MODE_CIRCLING;

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

void Application::stopCircling()
{
	// now normal flying
	//contextPtr->flightState.flightMode = FMODE_FLYING;
    deviceMode = MODE_FLYING;

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

void Application::startGliding()
{
	// start of circling
	//contextPtr->flightState.flightMode = FMODE_GLIDING;
    deviceMode = MODE_GLIDING;

	//
	contextPtr->flightState.glidingStartPos.lon = contextPtr->varioState.longitude;
	contextPtr->flightState.glidingStartPos.lat = contextPtr->varioState.latitude;
	contextPtr->flightState.glidingStartPos.alt = contextPtr->varioState.altitudeGPS;

	contextPtr->flightState.glideRatio = -1;
}

void Application::stopGliding()
{
	// now normal flying
	//contextPtr->flightState.flightMode = FMODE_FLYING;
    deviceMode = MODE_FLYING;

	//
	contextPtr->flightState.glideRatio = -1;
}

void Application::sendMessage(uint32_t code, uint32_t data)
{
    Message msg = { code, data };
    xQueueSend(msgQueue, &msg, 5);
}


//
// IKeypadCallback
//


void Application::onPressed(uint8_t key)
{
    LOGv("Key Pressed: %d", key);
    sendMessage(MSG_KEY_PRESSED, key);
}

void Application::onLongPressed(uint8_t key)
{
    if (key == KEY_PUSH)
    {
        // prepare shutdown
        prepareShutdown = true;

        LOGi("Send SHUTDOWN message");
        sendMessage(MSG_SHUTDOWN);
    }
    else
    {
        LOGv("Key Long-Pressed: %d", key);
        sendMessage(MSG_KEY_LONG_PRESSED, key);
    }
}

void Application::onReleased(uint8_t key)
{
    LOGv("Key Released: %d", key);
    sendMessage(MSG_KEY_RELEASED, key);
}


//
// Tasks
//


void Application::ScreenTask(void* param)
{
    // suspend this task and wait resume
    vTaskSuspend(NULL);

    Application* pThis = (Application *)param;    
    pThis->ScreenTask();
    
    vTaskDelete(NULL);
}

void Application::DeviceTask(void* param)
{
    // suspend this task and wait resume
    vTaskSuspend(NULL);

    Application* pThis = (Application *)param;
    pThis->DeviceTask();

    vTaskDelete(NULL);
}

void Application::ScreenTask()
{
    //
    Window* active = Scrn.getActiveWindow();
    active->update(contextPtr, 0);
    //active->onDraw();

    //
    uint32_t tickCount = 0;

    while (true)
    {
        Message msg;
        if (xQueueReceive(msgQueue, &msg, 100))
        {
            LOGd("MSG: code(%d), data(%d)", msg.code, msg.data);
            if (msg.code == MSG_SHUTDOWN)
                break; // exit loop

            //
            active = Scrn.getActiveWindow();
            if (active != nullptr)
            {
                switch (msg.code)
                {
                case MSG_KEY_PRESSED:
                    active->onKeyPressed(msg.data);
                    break;
                case MSG_KEY_LONG_PRESSED:
                    active->onKeyLongPressed(msg.data);
                    break;
                case MSG_KEY_RELEASED:
                    active->onKeyReleased(msg.data);
                    break;
                case MSG_TOUCH_DOWN:
                    active->onTouchDown(msg.data >> 16, msg.data & 0x00FF);
                    break;
                case MSG_TOUCH_MOVE:
                    active->onTouchMove(msg.data >> 16, msg.data & 0x00FF);
                    break;
                case MSG_TOUCH_UP:
                    active->onTouchUp(msg.data >> 16, msg.data & 0x00FF);
                    break;
                default:
                    active->onMessage(msg.code, msg.data);
                    break;
                }
            }
        }
        else
        {
            active = Scrn.getActiveWindow();
            if (active != nullptr)
                active->onIdle();
        }
    }

    //
    {
        // notify shutdown
        if (0)
        {
            #define NOTI_WIDTH  420
            #define NOTI_HEIGHT 80

            int x = (LCD_WIDTH - NOTI_WIDTH) / 2;
            int y = (LCD_HEIGHT - NOTI_HEIGHT) / 2;

            Display.fillRect(x, y, NOTI_WIDTH, NOTI_HEIGHT, M5EPD_Canvas::G0);
            Display.drawRect(x, y, NOTI_WIDTH, NOTI_HEIGHT, M5EPD_Canvas::G15);

            Display.setTextDatum(CC_DATUM);
            Display.setTextSize(48);
            Display.setTextColor(M5EPD_Canvas::G15);
            Display.drawString("Shutdown Deivce", x + NOTI_WIDTH / 2, y + NOTI_HEIGHT / 2);
            Display.pushCanvas(0, 0, UPDATE_MODE_DU4);

            LOGi("Wait device shutdown");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }

        // draw shutdown image
        {
            LOGv("Draw shutdown image");

            // clar & draw power-off screen
            EPD.Clear(true);

            #define LOGO_WIDTH  320
            #define LOG_HEIGHT  320

            int x = (LCD_WIDTH - LOGO_WIDTH) / 2;
            int y = (LCD_HEIGHT - LOG_HEIGHT) / 2;

            Display.clear();

            Display.pushImage(x, y, LOGO_WIDTH, LOG_HEIGHT, ImageResource_logo_eagle_320x320);
            Display.pushCanvas(0, 0, UPDATE_MODE_GLD16);

            delay(1000);
        }

        // power-off
        digitalWrite(KEY_POWER_EXT, 0);
        digitalWrite(KEY_POWER_EPD, 0);        
        delay(100);
        digitalWrite(KEY_POWER_MAIN, 0);
        while(1);
    }
}

void Application::DeviceTask()
{
    #define LOOP_PERIOD         (100)
    #define CHECK_INTERVAL(x)   ((x)/(LOOP_PERIOD))

    const TickType_t xDelay = LOOP_PERIOD / portTICK_PERIOD_MS;
    int BAT_delayCount = 0;
    int TH_delayCount = 0;

    while (1)
    {
        if (prepareShutdown)
            break;

        // BAT
        if (++BAT_delayCount > CHECK_INTERVAL(200))
        {
            if (BAT.update() > 0)
            {
                // update battery voltage
                float volt = BAT.getVoltage();
                LOGd("Battery: %.2fv", volt);

                contextPtr->deviceState.batteryPower = volt;

                //
                sendMessage(MSG_UPDATE_BAT);
            }

            BAT_delayCount = 0;
        }

        // TH
        if (++TH_delayCount > CHECK_INTERVAL(1000))
        {
            if (TH.UpdateData() == 0)
            {
                // update temperature & humidity
                float tem = TH.GetTemperature();
                float hum = TH.GetRelHumidity();

                contextPtr->deviceState.temperature = tem;
                contextPtr->deviceState.humidity = hum;
                LOGd("Temperature: %.2f*C, Humidity: %.2f%%", tem, hum);

                // notify update
                sendMessage(MSG_UPDATE_TH);
            }

            TH_delayCount = 0;
        }

        // TP
        // ...

        // KEY
        KEY.update();

        vTaskDelay(xDelay);
    }
}
