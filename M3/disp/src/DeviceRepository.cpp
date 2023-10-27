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
		const size_t capacity = JSON_OBJECT_SIZE(45) + 1240;
		DynamicJsonDocument doc(capacity);

		DeserializationError error = deserializeJson(doc, file);
		LOGv("config.json deserialize => %s(%d)", error.c_str(), error.code());
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
	contextPtr->userSettings.gliderType = GTYPE_PARAGLIDER;

	memset(contextPtr->userSettings.gliderManufacture, 0, sizeof(contextPtr->userSettings.gliderManufacture));
	memset(contextPtr->userSettings.gliderModel, 0, sizeof(contextPtr->userSettings.gliderModel));
	
	//
	contextPtr->deviceSettings.enableLogging = true;
	
	contextPtr->deviceSettings.takeoffSpeed = FLIGHT_START_MIN_SPEED;		// km/s
	contextPtr->deviceSettings.landingSpeed = FLIGHT_STOP_MIN_SPEED;		// km/s
	contextPtr->deviceSettings.landingTimeout = FLIGHT_LANDING_THRESHOLD;	// ms
	contextPtr->deviceSettings.loggingInterval = FLIGHT_LOGGING_INTERVAL;	// ms
	
	memset(contextPtr->userSettings.pilot, 0, sizeof(contextPtr->userSettings.pilot));
	
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
	contextPtr->deviceSettings.volume = contextPtr->deviceState.volume = VARIOMETER_BEEP_VOLUME;
	contextPtr->deviceSettings.autoSoundOn = 1;
	
	//
	contextPtr->deviceSettings.lowBattery = LOW_BATTERY_THRESHOLD;
	contextPtr->deviceSettings.shutdownHoldtime = SHUTDOWN_HOLD_TIME;
	contextPtr->deviceSettings.shutdownTimeout = AUTO_SHUTDOWN_THRESHOLD;
	
	//
#if OBSOLETE
	contextPtr->kalman.varZMeas = KF_ZMEAS_VARIANCE;
	contextPtr->kalman.varZAccel = KF_ZACCEL_VARIANCE;
	contextPtr->kalman.varAccelBias = KF_ACCELBIAS_VARIANCE;
#endif

	//
	contextPtr->deviceSettings.enableBT = 1;
	contextPtr->deviceSettings.enableSound = 1;
	contextPtr->deviceSettings.enableSimulation = 0;
	contextPtr->deviceSettings.enableNmeaLogging = 0;

    contextPtr->deviceSettings.timezone = VARIOMETER_TIME_ZONE; 			// GMT+9	
	contextPtr->deviceState.timezoneOffset = (time_t)(VARIOMETER_TIME_ZONE * 3600.0f);
	contextPtr->deviceSettings.compassUpside = CompassUpside::UP_HEADING;

	strcpy(contextPtr->deviceSettings.btName, "CubeM3-001");
	strcpy(contextPtr->deviceSettings.wifiSSID, "CubeM3");
	strcpy(contextPtr->deviceSettings.wifiPassword, "1234567890");

	//
	contextPtr->deviceState.batteryPower = 0;
	contextPtr->deviceState.temperature = 0;
	contextPtr->deviceState.humidity = 0;

	contextPtr->deviceState.statusGPS = 0;
	contextPtr->deviceState.statusBT = 0; // contextPtr->deviceDefault.enableBT ? 1 : 0;
	contextPtr->deviceState.statusSDCard = 0;

	//contextPtr->deviceState.ipAddr[0] = '\0';
	//contextPtr->deviceState.wifiState = 0;

	//
	contextPtr->flightState.bearingTakeoff = -1;
	contextPtr->flightState.glidingRatio = -1;
}


size_t DeviceRepository::updateVSpeedHistory(float speed) 
{
	VarioState& state = contextPtr->varioState;

	// push to front
	state.speedVertHistory.value[state.speedVertHistory.front] = speed;
	// forward front-index
	state.speedVertHistory.front = (state.speedVertHistory.front + 1) & (MAX_VARIO_HISTORY - 1); // % MAX_VARIO_HISTORY;
	// check full
	if (state.speedVertHistory.front == state.speedVertHistory.rear)
		state.speedVertHistory.rear = (state.speedVertHistory.rear + 1) & (MAX_VARIO_HISTORY - 1); // % MAX_VARIO_HISTORY;

	return (state.speedVertHistory.front - state.speedVertHistory.rear) & (MAX_VARIO_HISTORY - 1);
}

size_t DeviceRepository::updateTrackHistory() 
{
	FlightState &state = contextPtr->flightState;
	VarioState &vario = contextPtr->varioState;
	int16_t latest = state.trackHistory.front;

	//
	TrackPoint &point = state.trackHistory.points[state.trackHistory.front];
	point.lat = vario.latitude;
	point.lon = vario.longitude;
	point.alt = vario.altitudeGPS;
	point.vario = vario.speedVertActive;

	state.trackHistory.front = (state.trackHistory.front + 1) & (MAX_TRACK_HISTORY - 1);

	if (state.trackHistory.rear == state.trackHistory.front) // full ? : drop oldest
		state.trackHistory.rear = (state.trackHistory.rear + 1) & (MAX_TRACK_HISTORY - 1);

	// calculate relative distance : distance from latest point to each point
	//int latest = (state.trackHistory.front + MAX_TRACK_HISTORY - 1) % MAX_TRACK_HISTORY;
	for (int i = state.trackHistory.rear; i != latest; )
	{
		state.trackHistory.dist[i].dx = -GET_DISTANCE(state.trackHistory.points[latest].lon - state.trackHistory.points[i].lon);
		state.trackHistory.dist[i].dy = GET_DISTANCE(state.trackHistory.points[latest].lat - state.trackHistory.points[i].lat);

		i = (i + 1) & (MAX_TRACK_HISTORY - 1);
	}

	state.trackHistory.dist[latest].dx = 0;
	state.trackHistory.dist[latest].dy = 0;

	//LOGv("TrackPoint(%f,%f,%f) [%d-%d]", lat, lon, speedVert, state.rearPoint, state.frontPoint);
	return (state.trackHistory.front - state.trackHistory.rear) & (MAX_TRACK_HISTORY - 1);
}

size_t DeviceRepository::getVSpeedCount()
{
	VarioState& state = contextPtr->varioState;
	return (state.speedVertHistory.front - state.speedVertHistory.rear) & (MAX_VARIO_HISTORY - 1);
}

size_t DeviceRepository::getTrackCount()
{
	FlightState& state = contextPtr->flightState;
	return (state.trackHistory.front - state.trackHistory.rear) & (MAX_TRACK_HISTORY - 1);
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
	if (! doc["vario_climb_threshold"].isUnbound())
		contextPtr->varioSettings.climbThreshold = doc["vario_climb_threshold"]; // 0.2
	if (! doc["vario_sink_threshold"].isUnbound())
		contextPtr->varioSettings.sinkThreshold = doc["vario_sink_threshold"]; // -3
	if (! doc["vario_sensitivity"].isUnbound())
		contextPtr->varioSettings.sensitivity = doc["vario_sensitivity"]; // 0.12
	if (! doc["vario_ref_altitude_1"].isUnbound())
		contextPtr->varioSettings.altitudeRef1 = doc["vario_ref_altitude_1"]; // 0
	if (! doc["vario_ref_altitude_2"].isUnbound())
		contextPtr->varioSettings.altitudeRef2 = doc["vario_ref_altitude_2"]; // 0
	if (! doc["vario_ref_altitude_3"].isUnbound())
		contextPtr->varioSettings.altitudeRef3 = doc["vario_ref_altitude_3"]; // 0
	if (! doc["vario_damping_factor"].isUnbound())
		contextPtr->varioSettings.dampingFactor = doc["vario_damping_factor"]; // 0.05
	if (! doc["glider_type"].isUnbound())
		contextPtr->userSettings.gliderType = doc["glider_type"]; // 1
	if (! doc["glider_manufacture"].isUnbound())
		strcpy(contextPtr->userSettings.gliderManufacture, (const char *)doc["glider_manufacture"]); // "Ozone"
	if (! doc["glider_model"].isUnbound())
		strcpy(contextPtr->userSettings.gliderModel, (const char *)doc["glider_model"]); // "Zeno"
	if (! doc["igc_enable_logging"].isUnbound())
		contextPtr->deviceSettings.enableLogging = doc["igc_enable_logging"]; // true
	if (! doc["igc_takeoff_speed"].isUnbound())
		contextPtr->deviceSettings.takeoffSpeed = doc["igc_takeoff_speed"]; // 6
	if (! doc["igc_landing_speed"].isUnbound())
		contextPtr->deviceSettings.landingSpeed = doc["igc_landing_speed"]; // 2
	if (! doc["igc_landing_timeout"].isUnbound())
		contextPtr->deviceSettings.landingTimeout = doc["igc_landing_timeout"]; // 10000
	if (! doc["igc_logging_interval"].isUnbound())
		contextPtr->deviceSettings.loggingInterval = doc["igc_logging_interval"]; // 1000
	if (! doc["igc_pilot"].isUnbound())
		strcpy(contextPtr->userSettings.pilot, (const char *)doc["igc_pilot"]); // "akkdong"
	if (! doc["volume_enable_vario"].isUnbound())
		contextPtr->deviceSettings.volume = contextPtr->deviceState.volume = doc["volume_enable_vario"] ? 100 : 0; // false
	#if OBSOLETE
	if (! doc["volume_enable_effect"].isUnbound())
		contextPtr->deviceSettings.effect = contextPtr->deviceState.effect = doc["volume_enable_effect"] ? 100 : 0; // false
	#endif
	if (! doc["volume_auto_turnon"].isUnbound())
		contextPtr->deviceSettings.autoSoundOn = doc["volume_auto_turnon"]; // true
	if (! doc["threshold_low_battery"].isUnbound())
		contextPtr->deviceSettings.lowBattery = doc["threshold_low_battery"]; // 2.9
	if (! doc["threshold_auto_shutdown"].isUnbound())
		contextPtr->deviceSettings.shutdownTimeout = doc["threshold_auto_shutdown"]; // 600000
#if OBSOLETE
	if (! doc["kalman_var_zmeas"].isUnbound())
		contextPtr->kalman.varZMeas = doc["kalman_var_zmeas"]; // 400
	if (! doc["kalman_var_zaccel"].isUnbound())
		contextPtr->kalman.varZAccel = doc["kalman_var_zaccel"]; // 1000
	if (! doc["kalman_var_abias"].isUnbound())
		contextPtr->kalman.varAccelBias = doc["kalman_var_abias"]; // 1
#endif
	if (! doc["device_enable_bt"].isUnbound())
		contextPtr->deviceSettings.enableBT = doc["device_enable_bt"]; // false
	if (! doc["device_enable_sound"].isUnbound())
		contextPtr->deviceSettings.enableSound = doc["device_enable_sound"]; // false
	if (! doc["device_bt_name"].isUnbound())
		strcpy(contextPtr->deviceSettings.btName, (const char *)doc["device_bt_name"]); // "MiniVario"
	if (! doc["device_enable_simulation"].isUnbound())
		contextPtr->deviceSettings.enableSimulation = doc["device_enable_simulation"]; // false
	if (! doc["device_enable_nmea_logging"].isUnbound())
		contextPtr->deviceSettings.enableNmeaLogging = doc["device_enable_nmea_logging"]; // false
	if (! doc["wifi_ssid"].isUnbound())
		strcpy(contextPtr->deviceSettings.wifiSSID, (const char *)doc["wifi_ssid"]); // "MiniVario"
	if (! doc["wifi_password"].isUnbound())
		strcpy(contextPtr->deviceSettings.wifiPassword, (const char *)doc["wifi_password"]); // "123456789"
	if (! doc["timezone"].isUnbound())
	{
		contextPtr->deviceSettings.timezone = doc["timezone"]; // 9
		contextPtr->deviceState.timezoneOffset = (time_t)(contextPtr->deviceSettings.timezone * 3600.0f);
	}
	if (! doc["compass_up"].isUnbound())
		contextPtr->deviceSettings.compassUpside = doc["compass_up"];
}

void DeviceRepository::dump()
{
	LOGv("DeviceDefault.enableBT = %d", contextPtr->deviceSettings.enableBT);
	LOGv("DeviceDefault.enableSound = %d", contextPtr->deviceSettings.enableSound);
	LOGv("DeviceDefault.enableSimulation = %d", contextPtr->deviceSettings.enableSimulation);
	LOGv("DeviceDefault.enableNmeaLogging = %d", contextPtr->deviceSettings.enableNmeaLogging);
	LOGv("DeviceDefault.btName = %s", contextPtr->deviceSettings.btName);
	LOGv("DeviceDefault.wifiSSID = %s", contextPtr->deviceSettings.wifiSSID);
	LOGv("DeviceDefault.wifiPassword = %s", contextPtr->deviceSettings.wifiPassword);
	LOGv("DeviceDefault.timezone = %f", contextPtr->deviceSettings.timezone);
	LOGv("VolumeSettings.autoSoundOn = %d", contextPtr->deviceSettings.autoSoundOn);
	LOGv("VolumeSettings.vario = %d", contextPtr->deviceSettings.volume);

	LOGv("VarioSettings.sinkThreshold = %f", contextPtr->varioSettings.sinkThreshold);
	LOGv("VarioSettings.climbThreshold = %f", contextPtr->varioSettings.climbThreshold);
	LOGv("VarioSettings.sensitivity = %f", contextPtr->varioSettings.sensitivity);
	LOGv("VarioSettings.sentence = %d", contextPtr->varioSettings.sentence);
	LOGv("VarioSettings.altitudeRef1 = %f", contextPtr->varioSettings.altitudeRef1);
	LOGv("VarioSettings.altitudeRef2 = %f", contextPtr->varioSettings.altitudeRef2);
	LOGv("VarioSettings.altitudeRef3 = %f", contextPtr->varioSettings.altitudeRef3);
	LOGv("VarioSettings.dampingFactor = %f", contextPtr->varioSettings.dampingFactor);

#if OBSOLETE
	LOGv("KalmanParameter.varZMeas = %f", contextPtr->kalman.varZMeas);
	LOGv("KalmanParameter.varZAccel = %f", contextPtr->kalman.varZAccel);
	LOGv("KalmanParameter.varAccelBias = %f", contextPtr->kalman.varAccelBias);
#endif

	LOGv("GliderInfo.type = %d", contextPtr->userSettings.gliderType);
	LOGv("GliderInfo.manufacture = %s", contextPtr->userSettings.gliderManufacture);
	LOGv("GliderInfo.model = %s", contextPtr->userSettings.gliderModel);

	LOGv("IGCLogger.enable = %d", contextPtr->deviceSettings.enableLogging);
	LOGv("IGCLogger.takeoffSpeed = %d", contextPtr->deviceSettings.takeoffSpeed);
	LOGv("IGCLogger.landingSpeed = %d", contextPtr->deviceSettings.landingSpeed);
	LOGv("IGCLogger.landingTimeout = %d", contextPtr->deviceSettings.landingTimeout);
	LOGv("IGCLogger.loggingInterval = %d", contextPtr->deviceSettings.loggingInterval);
	LOGv("IGCLogger.pilot = %s", contextPtr->userSettings.pilot);
}
