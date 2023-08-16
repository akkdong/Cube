// DeviceRepository.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#endif

#include "board.h"
#include "logger.h"

#include "DeviceRepository.h"


#define RADIUS 					(6371000) // 6371e3

#define TO_RADIAN(x)			((x)*(PI/180))
#define GET_DISTANCE(angle)		(2.0 * RADIUS * sin(TO_RADIAN((angle) / 2)))


////////////////////////////////////////////////////////////////////////////////////////////////
// 

#if OBSOLETE
const VarioTone DeviceRepository::defaultTone[] =
{
	// y = exp((x + 320) / 180) x 0.8 + 800
	//
	{  -10.00,   200, 200,  100 },
	{   -3.00,   290, 200,  100 },
	{   -2.00,   369, 200,  100 },
	{   -1.00,   440, 200,  100 },
	{   -0.50,   470, 600,  100 },
	{    0.00,   523, 800,   50 },	// C5
	{    1.00,   622, 500,   54 },	// D5S
	{    2.00,   698, 400,   58 },	// F5
	{    4.00,   784, 275,   62 },	// G5
	{    6.00,   880, 250,   66 },	// A5
	{    8.00,   988, 225,   68 },	// B5
	{   10.00,  1047, 200,   70 },	// C6
};
#endif // OBSOLETE


////////////////////////////////////////////////////////////////////////////////////////////////
// class DeviceRepository

DeviceRepository::DeviceRepository()
{
    #ifdef ARDUINO
    contextPtr = (DeviceContext *)ps_malloc(sizeof(DeviceContext));
    #else
    contextPtr = (DeviceContext *)malloc(sizeof(DeviceContext));
    #endif
}

DeviceRepository& DeviceRepository::instance()
{
	static DeviceRepository _repo;

    return _repo;
}

bool DeviceRepository::loadPref()
{
    #ifdef ARDUINO
	File file = SPIFFS.open("/config.json");
	if (file)
	{
		const size_t capacity = JSON_OBJECT_SIZE(31) + 700;
		DynamicJsonDocument doc(capacity);

		DeserializationError error = deserializeJson(doc, file);
		file.close();

		if (! error)
			set(doc);

		#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
		dump();
		#endif
	}	
    return true;
    #else
    return true;
    #endif
}

bool DeviceRepository::savePref()
{
    #ifdef ARDUINO
    JsonDocument* doc;

    return true;
    #else
    return true;
    #endif
}

void DeviceRepository::reset()
{
    if (!contextPtr)
        return;

	memset(contextPtr, 0, sizeof(DeviceContext));
	
	//
	contextPtr->gliderInfo.type = GTYPE_PARAGLIDER;
	
	memset(contextPtr->gliderInfo.manufacture, 0, sizeof(contextPtr->gliderInfo.manufacture));
	memset(contextPtr->gliderInfo.model, 0, sizeof(contextPtr->gliderInfo.model));
	
	//
	contextPtr->logger.enable = true;
	
	contextPtr->logger.takeoffSpeed = FLIGHT_START_MIN_SPEED;		// km/s
	contextPtr->logger.landingSpeed = FLIGHT_STOP_MIN_SPEED;		// km/s
	contextPtr->logger.landingTimeout = FLIGHT_LANDING_THRESHOLD;	// ms
	contextPtr->logger.loggingInterval = FLIGHT_LOGGING_INTERVAL;	// ms
	
	memset(contextPtr->logger.pilot, 0, sizeof(contextPtr->logger.pilot));
	//contextPtr->logger.timezone = VARIOMETER_TIME_ZONE; 			// GMT+9	
	
	//
	contextPtr->varioSettings.sinkThreshold = VARIOMETER_SINKING_THRESHOLD; // -3.0
	contextPtr->varioSettings.climbThreshold = VARIOMETER_CLIMBING_THRESHOLD; // 0.2
	contextPtr->varioSettings.sensitivity = VARIOMETER_SENSITIVITY; // 0.12
	
	contextPtr->varioSettings.sentence = VARIOMETER_DEFAULT_NMEA_SENTENCE;

	contextPtr->varioSettings.dampingFactor	= VARIOMETER_DEFAULT_DAMPING_FACTOR; // 0.1

	// vario_tone_table 
#if OBSOLETE
	memcpy(&contextPtr->toneTable[0], &defaultTone[0], sizeof(defaultTone));
#endif
	
	//
	contextPtr->volume.varioDefault = contextPtr->volume.vario = VARIOMETER_BEEP_VOLUME;
	contextPtr->volume.effectDefault = contextPtr->volume.effect = VARIOMETER_EFFECT_VOLUME;
	contextPtr->volume.autoTurnOn = 1;
	
	//
	contextPtr->threshold.lowBattery = LOW_BATTERY_THRESHOLD;
	contextPtr->threshold.shutdownHoldtime = SHUTDOWN_HOLD_TIME;
	contextPtr->threshold.autoShutdownVario = AUTO_SHUTDOWN_THRESHOLD;
	
	//
#if OBSOLETE
	contextPtr->kalman.varZMeas = KF_ZMEAS_VARIANCE;
	contextPtr->kalman.varZAccel = KF_ZACCEL_VARIANCE;
	contextPtr->kalman.varAccelBias = KF_ACCELBIAS_VARIANCE;
#endif // OBSOLETE

	//
	contextPtr->deviceDefault.enableBT = 1;
	contextPtr->deviceDefault.enableSound = 1;
	contextPtr->deviceDefault.enableSimulation = 0;
	contextPtr->deviceDefault.enableNmeaLogging = 0;

    contextPtr->deviceDefault.timezone = VARIOMETER_TIME_ZONE; 			// GMT+9	
	contextPtr->deviceDefault.timezoneOffset = (long)(VARIOMETER_TIME_ZONE * 3600);

	strcpy(contextPtr->deviceDefault.btName, "M3-001");
	strcpy(contextPtr->deviceDefault.wifiSSID, "");
	strcpy(contextPtr->deviceDefault.wifiPassword, "");

	//
	contextPtr->deviceState.statusBT = contextPtr->deviceDefault.enableBT ? 1 : 0;

	//
	contextPtr->flightState.bearingTakeoff = -1;
	contextPtr->flightState.glideRatio = -1;
}


size_t DeviceRepository::updateVSpeedHistory(float speed) 
{
	VarioState& state = contextPtr->varioState;

	// push to front
	state.speedVertHistory[state.historyFront] = speed;
	// forward front-index
	state.historyFront = (state.historyFront + 1) & (MAX_VARIO_HISTORY - 1); // % MAX_VARIO_HISTORY;
	// check full
	if (state.historyFront == state.historyRear)
		state.historyRear = (state.historyRear + 1) & (MAX_VARIO_HISTORY - 1); // % MAX_VARIO_HISTORY;

	return (state.historyFront - state.historyRear) & (MAX_VARIO_HISTORY - 1);
}

size_t DeviceRepository::updateTrackHistory(float lat, float lon, float speedVert) 
{
	FlightState& state = contextPtr->flightState;
	int16_t latest = state.frontPoint;

	// 
	state.trackPoints[state.frontPoint] = TrackPoint(lat, lon, speedVert);
	state.frontPoint = (state.frontPoint + 1) & (MAX_TRACK_HISTORY - 1);

	if (state.rearPoint == state.frontPoint)
		state.rearPoint = (state.rearPoint + 1) & (MAX_TRACK_HISTORY - 1);

	// calculate relative distance : distance from latest point to each point
	//int latest = (state.frontPoint + MAX_TRACK_HISTORY - 1) % MAX_TRACK_HISTORY;
	for (int i = state.rearPoint; i != latest; )
	{
		state.trackDistance[i].dx = -GET_DISTANCE(state.trackPoints[latest].lon - state.trackPoints[i].lon);
		state.trackDistance[i].dy = GET_DISTANCE(state.trackPoints[latest].lat - state.trackPoints[i].lat);

		i = (i + 1) & (MAX_TRACK_HISTORY - 1);
	}

	state.trackDistance[latest].dx = 0;
	state.trackDistance[latest].dy = 0;

	//LOGv("TrackPoint(%f,%f,%f) [%d-%d]", lat, lon, speedVert, state.rearPoint, state.frontPoint);
	return (state.frontPoint - state.rearPoint) & (MAX_TRACK_HISTORY - 1);
}

#if OBSOLETE
size_t DeviceRepository::updateTrackHistory()
{
	FlightState& state = contextPtr->flightState;
	VarioState& vario = contextPtr->varioState;

	// add new track-position
	state.trackHistory[state.frontPoint] = TrackHistory(vario.latitude, vario.longitude, vario.heading, state.distFlight, vario.speedVertLazy);
	//LOGv("Track: %f, %f, %d, %f, %f", vario.latitude, vario.longitude, vario.heading, state.distFlight, vario.speedVertLazy);

	state.frontPoint = (state.frontPoint + 1) & (MAX_TRACK_HISTORY - 1);
	if (state.frontPoint == state.rearPoint) // overflow
		state.rearPoint = (state.rearPoint + 1) & (MAX_TRACK_HISTORY - 1);

	int16_t size = (state.frontPoint - state.rearPoint) & (MAX_TRACK_HISTORY - 1);
	LOGv("track-history size: %d", size);
	return size;
}
#endif

size_t DeviceRepository::getVSpeedCount()
{
	VarioState& state = contextPtr->varioState;
	return (state.historyFront - state.historyRear) & (MAX_VARIO_HISTORY - 1);
}

size_t DeviceRepository::getTrackCount()
{
	FlightState& state = contextPtr->flightState;
	return (state.frontPoint - state.rearPoint) & (MAX_TRACK_HISTORY - 1);
}

void DeviceRepository::resetFlightState() 
{ 
	memset(&contextPtr->flightState, 0, sizeof(FlightState)); 
}

void DeviceRepository::resetFlightStats() 
{ 
	memset(&contextPtr->flightStats, 0, sizeof(FlightStats)); 
}



void DeviceRepository::set(JsonDocument& doc)
{
	if (! doc["vario_climb_threshold"].isNull())
		contextPtr->varioSettings.climbThreshold = doc["vario_climb_threshold"]; // 0.2
	if (! doc["vario_sink_threshold"].isNull())
		contextPtr->varioSettings.sinkThreshold = doc["vario_sink_threshold"]; // -3
	if (! doc["vario_sensitivity"].isNull())
		contextPtr->varioSettings.sensitivity = doc["vario_sensitivity"]; // 0.12
	if (! doc["vario_ref_altitude_1"].isNull())
		contextPtr->varioSettings.altitudeRef1 = doc["vario_ref_altitude_1"]; // 0
	if (! doc["vario_ref_altitude_2"].isNull())
		contextPtr->varioSettings.altitudeRef2 = doc["vario_ref_altitude_2"]; // 0
	if (! doc["vario_ref_altitude_3"].isNull())
		contextPtr->varioSettings.altitudeRef3 = doc["vario_ref_altitude_3"]; // 0
	if (! doc["vario_damping_factor"].isNull())
		contextPtr->varioSettings.dampingFactor = doc["vario_damping_factor"]; // 0.05
	if (! doc["glider_type"].isNull())
		contextPtr->gliderInfo.type = doc["glider_type"]; // 1
	if (! doc["glider_manufacture"].isNull())
		strcpy(contextPtr->gliderInfo.manufacture, (const char *)doc["glider_manufacture"]); // "Ozone"
	if (! doc["glider_model"].isNull())
		strcpy(contextPtr->gliderInfo.model, (const char *)doc["glider_model"]); // "Zeno"
	if (! doc["igc_enable_logging"].isNull())
		contextPtr->logger.enable = doc["igc_enable_logging"]; // true
	if (! doc["igc_takeoff_speed"].isNull())
		contextPtr->logger.takeoffSpeed = doc["igc_takeoff_speed"]; // 6
	if (! doc["igc_landing_speed"].isNull())
		contextPtr->logger.landingSpeed = doc["igc_landing_speed"]; // 2
	if (! doc["igc_landing_timeout"].isNull())
		contextPtr->logger.landingTimeout = doc["igc_landing_timeout"]; // 10000
	if (! doc["igc_logging_interval"].isNull())
		contextPtr->logger.loggingInterval = doc["igc_logging_interval"]; // 1000
	if (! doc["igc_pilot"].isNull())
		strcpy(contextPtr->logger.pilot, (const char *)doc["igc_pilot"]); // "akkdong"
	if (! doc["volume_enable_vario"].isNull())
		contextPtr->volume.varioDefault = contextPtr->volume.vario = doc["volume_enable_vario"] ? 100 : 0; // false
	if (! doc["volume_enable_effect"].isNull())
		contextPtr->volume.effectDefault = contextPtr->volume.effect = doc["volume_enable_effect"] ? 100 : 0; // false
	if (! doc["volume_auto_turnon"].isNull())
		contextPtr->volume.autoTurnOn = doc["volume_auto_turnon"]; // true
	if (! doc["threshold_low_battery"].isNull())
		contextPtr->threshold.lowBattery = doc["threshold_low_battery"]; // 2.9
	if (! doc["threshold_auto_shutdown"].isNull())
		contextPtr->threshold.autoShutdownVario = doc["threshold_auto_shutdown"]; // 600000
#if OBSOLETE
	if (! doc["kalman_var_zmeas"].isNull())
		contextPtr->kalman.varZMeas = doc["kalman_var_zmeas"]; // 400
	if (! doc["kalman_var_zaccel"].isNull())
		contextPtr->kalman.varZAccel = doc["kalman_var_zaccel"]; // 1000
	if (! doc["kalman_var_abias"].isNull())
		contextPtr->kalman.varAccelBias = doc["kalman_var_abias"]; // 1
#endif
	if (! doc["device_enable_bt"].isNull())
		contextPtr->deviceDefault.enableBT = doc["device_enable_bt"]; // false
	if (! doc["device_enable_sound"].isNull())
		contextPtr->deviceDefault.enableSound = doc["device_enable_sound"]; // false
	if (! doc["device_bt_name"].isNull())
		strcpy(contextPtr->deviceDefault.btName, (const char *)doc["device_bt_name"]); // "MiniVario"
	if (! doc["device_enable_simulation"].isNull())
		contextPtr->deviceDefault.enableSimulation = doc["device_enable_simulation"]; // false
	if (! doc["device_enable_nmea_logging"].isNull())
		contextPtr->deviceDefault.enableNmeaLogging = doc["device_enable_nmea_logging"]; // false
	if (! doc["wifi_ssid"].isNull())
		strcpy(contextPtr->deviceDefault.wifiSSID, (const char *)doc["wifi_ssid"]); // "MiniVario"
	if (! doc["wifi_password"].isNull())
		strcpy(contextPtr->deviceDefault.wifiPassword, (const char *)doc["wifi_password"]); // "123456789"
	if (! doc["timezone"].isNull())
	{
		contextPtr->deviceDefault.timezone = doc["wifi_password"]; // 9
		contextPtr->deviceDefault.timezoneOffset = (long)(contextPtr->deviceDefault.timezone * 3600);
	}
}

void DeviceRepository::dump()
{
	Serial.printf("DeviceDefault.enableBT = %d\r\n", contextPtr->deviceDefault.enableBT);
	Serial.printf("DeviceDefault.enableSound = %d\r\n", contextPtr->deviceDefault.enableSound);
	Serial.printf("DeviceDefault.enableSimulation = %d\r\n", contextPtr->deviceDefault.enableSimulation);
	Serial.printf("DeviceDefault.enableNmeaLogging = %d\r\n", contextPtr->deviceDefault.enableNmeaLogging);
	Serial.printf("DeviceDefault.btName = %s\r\n", contextPtr->deviceDefault.btName);
	Serial.printf("DeviceDefault.wifiSSID = %s\r\n", contextPtr->deviceDefault.wifiSSID);
	Serial.printf("DeviceDefault.wifiPassword = %s\r\n", contextPtr->deviceDefault.wifiPassword);
	Serial.printf("DeviceDefault.timezone = %f\r\n", contextPtr->deviceDefault.timezone);

	Serial.printf("VolumeSettings.vario = %d\r\n", contextPtr->volume.vario);
	Serial.printf("VolumeSettings.effect = %d\r\n", contextPtr->volume.effect);
	Serial.printf("VolumeSettings.autoTurnOn = %d\r\n", contextPtr->volume.autoTurnOn);

	Serial.printf("VarioSettings.sinkThreshold = %f\r\n", contextPtr->varioSettings.sinkThreshold);
	Serial.printf("VarioSettings.climbThreshold = %f\r\n", contextPtr->varioSettings.climbThreshold);
	Serial.printf("VarioSettings.sensitivity = %f\r\n", contextPtr->varioSettings.sensitivity);
	Serial.printf("VarioSettings.sentence = %d\r\n", contextPtr->varioSettings.sentence);
	Serial.printf("VarioSettings.altitudeRef1 = %f\r\n", contextPtr->varioSettings.altitudeRef1);
	Serial.printf("VarioSettings.altitudeRef2 = %f\r\n", contextPtr->varioSettings.altitudeRef2);
	Serial.printf("VarioSettings.altitudeRef3 = %f\r\n", contextPtr->varioSettings.altitudeRef3);
	Serial.printf("VarioSettings.dampingFactor = %f\r\n", contextPtr->varioSettings.dampingFactor);

	//Serial.printf("KalmanParameter.varZMeas = %f\r\n", contextPtr->kalman.varZMeas);
	//Serial.printf("KalmanParameter.varZAccel = %f\r\n", contextPtr->kalman.varZAccel);
	//Serial.printf("KalmanParameter.varAccelBias = %f\r\n", contextPtr->kalman.varAccelBias);

	Serial.printf("GliderInfo.type = %d\r\n", contextPtr->gliderInfo.type);
	Serial.printf("GliderInfo.manufacture = %s\r\n", contextPtr->gliderInfo.manufacture);
	Serial.printf("GliderInfo.model = %s\r\n", contextPtr->gliderInfo.model);

	Serial.printf("IGCLogger.enable = %d\r\n", contextPtr->logger.enable);
	Serial.printf("IGCLogger.takeoffSpeed = %d\r\n", contextPtr->logger.takeoffSpeed);
	Serial.printf("IGCLogger.landingSpeed = %d\r\n", contextPtr->logger.landingSpeed);
	Serial.printf("IGCLogger.landingTimeout = %d\r\n", contextPtr->logger.landingTimeout);
	Serial.printf("IGCLogger.loggingInterval = %d\r\n", contextPtr->logger.loggingInterval);
	Serial.printf("IGCLogger.pilot = %s\r\n", contextPtr->logger.pilot);
}
