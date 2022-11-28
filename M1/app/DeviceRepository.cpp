// DeviceRepository.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#include <ArduinoJson.h>
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#endif

#include "device_defines.h"
#include "logger.h"

#include "DeviceRepository.h"


#define RADIUS 					(6371000) // 6371e3

#define TO_RADIAN(x)			((x)*(PI/180))
#define GET_DISTANCE(angle)		(2.0 * RADIUS * sin(TO_RADIAN((angle) / 2)))


////////////////////////////////////////////////////////////////////////////////////////////////
// 

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
    static DeviceRepository repo;
    return repo;
}

bool DeviceRepository::loadPref()
{
    #ifdef ARDUINO

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
	memcpy(&contextPtr->toneTable[0], &defaultTone[0], sizeof(defaultTone));
	
	//
	contextPtr->volume.varioDefault = contextPtr->volume.vario = VARIOMETER_BEEP_VOLUME;
	contextPtr->volume.effectDefault = contextPtr->volume.effect = VARIOMETER_EFFECT_VOLUME;
	contextPtr->volume.autoTurnOn = 1;
	
	//
	contextPtr->threshold.lowBattery = LOW_BATTERY_THRESHOLD;
	contextPtr->threshold.shutdownHoldtime = SHUTDOWN_HOLD_TIME;
	contextPtr->threshold.autoShutdownVario = AUTO_SHUTDOWN_THRESHOLD;
	
	//
	contextPtr->kalman.varZMeas = KF_ZMEAS_VARIANCE;
	contextPtr->kalman.varZAccel = KF_ZACCEL_VARIANCE;
	contextPtr->kalman.varAccelBias = KF_ACCELBIAS_VARIANCE;

	//
	contextPtr->deviceDefault.enableBT = 1;
	contextPtr->deviceDefault.enableSound = 1;
	contextPtr->deviceDefault.enableSimulation = 0;
	contextPtr->deviceDefault.enableNmeaLogging = 0;

    contextPtr->deviceDefault.timezone = VARIOMETER_TIME_ZONE; 			// GMT+9	

	strcpy(contextPtr->deviceDefault.btName, "Cube-001");
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
