// DeviceContext.h
//

#ifndef __DEVICE_CONTEXT_H__
#define __DEVICE_CONTEXT_H__

#include <time.h>

#define MAX_STRING_SIZE					(16)
#define MAX_VARIO_HISTORY				(64)
#define MAX_VARIO_TONE					(12)

#define MAX_TRACK_HISTORY				(64)




////////////////////////////////////////////////////////////////////////////////////////////////
// 

struct VarioState // VarioDeviceState
{
	//
	float			altitudeGPS;		// QNH
	float			altitudeBaro;		// QNE
	float			altitudeCalibrated; // Baro + (Baro <-> GPS differential : altitudeDrift)
	float			altitudeAGL;		// QFE (from terrain)
	float			altitudeRef1;		// QFE (from take-off)
	float			altitudeRef2;		// QFE (from landing)
	float			altitudeRef3;		// QFE (from any-altitude)

	float			altitudeStart;		// take-off altitude
	float			altitudeGround;		// 
	
//	float			altitudeDrift;
	
	//
//	float			altitudeAbtain;
	float			thermalTime;
	float			thermalGain;
//	float			glideRatio;	
	
	//
	float			speedGround;
	float			speedAir;
	float			speedVertActive;
	float			speedVertLazy;
//	float			speedVertMean2S;
//	float			speedVertStat;

	float			speedVertHistory[MAX_VARIO_HISTORY]; // 1s vertial speed history
	uint16_t		historyFront;
	uint16_t		historyRear;
	//float			speedVertSumTotal;
	//int16_t		speedVertSumCount;
	//int16_t		speedVertNext;

	//
	float			longitude;
	float			latitude;

	float			longitudeLast;
	float			latitudeLast;

//	float			longitudeStart;	// take-off longitude
//	float			latitudeStart;	// take-off latitude

//	float			distTakeoff;	// distance from take-off place
//	float			distFlight;		// total flight distance(odometer?)

	int16_t			heading;
	int16_t			headingLast;
//	int16_t			bearing;
	
	//
	float			pressure;
	float			pressureLazy;
	float			temperature;		// by barometer
	float			temperatureAlt;		// by thermometer

	//
	time_t			timeCurrent;
//	time_t			timeStart;
//	time_t			timeFly;
};

struct VarioSettings
{
	float			sinkThreshold;
	float			climbThreshold;
	float			sensitivity;
	
	uint8_t			sentence;			// 
	
	float			altitudeRef1;		// reference base altitude
	float			altitudeRef2;
	float			altitudeRef3;
	
	float			dampingFactor; 		//  affect to VarioState.speedVertLazy
};

struct GliderInfo
{
	uint8_t			type;
	char			manufacture[MAX_STRING_SIZE];
	char			model[MAX_STRING_SIZE];
};

struct IGCLoggerData
{
	uint8_t			enable;
	
	int				takeoffSpeed;
	int				landingSpeed;
	int				landingTimeout;
	int				loggingInterval;
	
	char			pilot[MAX_STRING_SIZE];
//	float			timezone;
};

struct VarioTone
{
	float			velocity;
	
	uint16_t		freq;
	uint16_t		period;
	uint16_t		duty;
};

struct VolumeSettings
{
	uint8_t			varioDefault;
	uint8_t			effectDefault;

	uint8_t			vario;
	uint8_t			effect;

	uint8_t			autoTurnOn; // turn-on then sound automatically when it start flying
};

struct ThresholdSettings
{
	float			lowBattery;
	uint32_t		shutdownHoldtime;

	uint32_t		autoShutdownVario;
};

struct KalmanParameters
{
	float			varZMeas;
	float			varZAccel;
	float			varAccelBias;
};

struct DeviceState
{
	float			batteryPower;
	float			temperature;
	float			humidity;
	
	uint8_t			statusGPS;		// 0: no-signal, 1: 
	uint8_t			statusBT;		// 0: disabled, 1: on-wait-client, 2: connected
	uint8_t			statusSDCard;	// 0: empty, 1: valid, 2: logging
};

struct DeviceDefault
{
	uint8_t			enableBT;
	uint8_t			enableSound;

	char			btName[MAX_STRING_SIZE];

	uint8_t			enableSimulation;
	uint8_t			enableNmeaLogging;

	char			wifiSSID[MAX_STRING_SIZE];
	char			wifiPassword[MAX_STRING_SIZE];

	float			timezone;
	long			timezoneOffset;
};

struct FlightStats
{
	float			altitudeMax;
	float			altitudeMin;

	float			varioMax;
	float			varioMin;

    float           avgSink;
    float           avgClimb;


	int16_t			totalThermaling;
	int16_t			thermalingMaxGain;
};

struct Position
{
	float			lat;
	float 			lon;
	float			alt;
};

struct TrackPoint
{
	TrackPoint() : lat(0.0f), lon(0.0f), vario(0.0f) {}
	TrackPoint(float _lat, float _lon, float _vario) : lat(_lat), lon(_lon), vario(_vario) {}
	TrackPoint(const TrackPoint& tp) : lat(tp.lat), lon(tp.lon), vario(tp.vario) {}

	float			lat;
	float			lon;
	float			vario;
};

struct TrackDistance
{
	TrackDistance() : dx(0.0f), dy(0.0f) {}
	TrackDistance(float _dx, float _dy) : dx(_dx), dy(_dy) {}
	TrackDistance(const TrackDistance& td) : dx(td.dx), dy(td.dy) {}

	float			dx;
	float			dy;
};

struct TrackHistory
{
	TrackHistory() : lat(0.0f), lon(0.0f), track(0.0f), dist(0.0f), vario(0.0f) {}
	TrackHistory(float _lat, float _lon, float _track, float _dst, float _spd) : lat(_lat), lon(_lon), track(_track), dist(_dst), vario(_spd) {}
	TrackHistory(const TrackHistory& th) : lat(th.lat), lon(th.lon), track(th.track), dist(th.dist), vario(th.vario) {}

	float			lat;
	float			lon;
	float			track;
	float			dist;
	float			vario;
};

enum FlightMode
{
	FMODE_READY,	// READY, LANDING
	FMODE_FLYING,
	FMODE_CIRCLING,
	FMODE_GLIDING,
};

struct FlightState
{
	//
	uint32_t		takeOffTime;
	Position		takeOffPos;
	uint32_t		flightTime;

	//
	uint32_t		circlingStartTime;
	Position		circlingStartPos; // latitude, longitude, altitude
	uint32_t		circlingTime;
	int32_t			circlingGain;
	int32_t			circlingIncline; // tilt, slope ??

	int16_t			deltaHeading_AVG;
	int16_t			deltaHeading_SUM;

	//
	TrackPoint		trackPoints[MAX_TRACK_HISTORY];
	TrackDistance	trackDistance[MAX_TRACK_HISTORY];
	TrackHistory	trackHistory[MAX_TRACK_HISTORY];
	int16_t			frontPoint;
	int16_t			rearPoint;

	//
	Position		glidingStartPos;
	int32_t			glidingCount;
	float			glideRatio; // L/D(Lift to Drag)

	//
	int16_t			bearingTakeoff;
	int16_t			bearingNextPoint;

	//
	float			distTakeoff;
	float			distFlight;
	float			distFlightAccum;
	float			distNextPoint;

	//
	//FlightMode		flightMode;
};

enum GliderType
{
	GTYPE_UNKNOWNM,
	GTYPE_PARAGLIDER,
	GTYPE_HG_FLEX,
	GTYPE_HG_RIGID,
	GTYPE_SAILPLANE,
	GTYPE_PPG_FOOT,
	GTYPE_PPG_WHEEL
};


////////////////////////////////////////////////////////////////////////////////
// DeviceContext

struct DeviceContext
{
    VarioSettings       varioSettings;
    GliderInfo          gliderInfo;
    IGCLoggerData       logger;

    VarioTone           toneTable[MAX_VARIO_TONE];

    VolumeSettings      volume;
    ThresholdSettings   threshold;

    KalmanParameters    kalman;

    DeviceDefault       deviceDefault;

    VarioState          varioState;
    DeviceState         deviceState;

    FlightState         flightState;
    FlightStats         flightStats;
};



#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif // __DEVICE_CONTEXT_H__
