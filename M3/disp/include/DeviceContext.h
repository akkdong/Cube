// DeviceContext.h
//

#ifndef __DEVICE_CONTEXT_H__
#define __DEVICE_CONTEXT_H__

#include <memory>
#include <time.h>

#define MAX_STRING_SIZE					(16)
#define MAX_VARIO_HISTORY				(64)
#define MAX_VARIO_TONE					(12)

#define MAX_TRACK_HISTORY				(64)



//
//
//

enum FlightMode
{
	FMODE_READY,	// READY, LANDING
	FMODE_FLYING,
	FMODE_CIRCLING,
	FMODE_GLIDING,
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

enum CompassUpside
{
	UP_NORTH,
	UP_HEADING,
	UP_TARGET
};

struct VarioTone
{
	float			velocity;
	
	uint16_t		freq;
	uint16_t		period;
	uint16_t		duty;
};


struct KalmanParameters
{
	float			varZMeas;
	float			varZAccel;
	float			varAccelBias;
};



struct Position
{
	float			lat;
	float 			lon;
	float			alt;
};

struct TrackPoint
{
	TrackPoint() : lat(0.0f), lon(0.0f), alt(0.0f), vario(0.0f) {}
	TrackPoint(float _lat, float _lon, float _alt, float _vario) : lat(_lat), lon(_lon), alt(_alt), vario(_vario) {}
	TrackPoint(const TrackPoint& tp) : lat(tp.lat), lon(tp.lon), alt(tp.alt), vario(tp.vario) {}

	float			lat;
	float			lon;
	float			alt;
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
    TrackHistory() {
        front = rear = 0;
    }

    TrackPoint      points[MAX_TRACK_HISTORY];
    TrackDistance   dist[MAX_TRACK_HISTORY];
    int             front, rear;
};

struct FloatFIFO {
    float value[MAX_VARIO_HISTORY];
    int front, rear;
};

/*
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
*/

class XcTask;

typedef std::shared_ptr<XcTask> XcTaskPtr;


////////////////////////////////////////////////////////////////////////////////////////////////
// 

struct DeviceSettings
{
    // enable on startup
	uint8_t			enableBT;
	uint8_t			enableSound;
    uint8_t			enableLogging;
	uint8_t			enableSimulation;
	uint8_t			enableNmeaLogging;

    uint8_t         autoSoundOn;

    // threshold settings
    float			lowBattery;
    uint32_t		shutdownHoldtime;
    uint32_t		shutdownTimeout;  

    int             takeoffSpeed;   // flight
    int             landingSpeed;   // flight
    int             landingTimeout; // flight
    int             loggingInterval;// logging

    // volume-settings
	uint8_t			volume;

	//
	char			wifiSSID[MAX_STRING_SIZE];
	char			wifiPassword[MAX_STRING_SIZE];

	char			btName[MAX_STRING_SIZE];

	//
	float			timezone;
	uint8_t			compassUpside; // north, heading, target
};

struct UserSettings
{
    char			pilot[MAX_STRING_SIZE];
    
	uint8_t			gliderType; // GliderType
	char			gliderManufacture[MAX_STRING_SIZE];
	char			gliderModel[MAX_STRING_SIZE];
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

    // tone-table
    // kalman parameters
};




struct DeviceState
{
	float			batteryPower;
	float			temperature;
	float			humidity;

    // volumn
	uint8_t			volume;
    uint8_t         mute;               // 0: sound-on, 1: sound-off
	
	uint8_t			statusGPS;		    // 0: unfixed, 1: fixed
	uint8_t			statusBT;		    // 0: disabled, 1: unparied, 2: paired
	uint8_t			statusSDCard;	    // 0: empty, 1: valid, 2: logging
    uint8_t			statusIGCLogging;   // 0: disable, 1: ready, 2: logging
    uint8_t			statusNmeaLogging;  // 0: disable, 1: enable
    uint8_t			statusSimulation;   // 0: disable, 1: enable

    time_t          timezoneOffset;

	char			ipAddr[16]; 	// xxx.xxx.xxx.xxx
	int				wifiState;		// 0: ready, 1: ap-started, 2: connected
};


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

	float			altitudeTakeoff;	// altitudeStart take-off altitude
	float			altitudeGround;		// AGL: Ground Level
	
//	float			altitudeDrift;
	
	//
	float			thermalTime;
	float			thermalGain;
//	float			altitudeAbtain;
//	float			glideRatio;	
	
	//
	float			speedGround;
	float			speedAir;
	float			speedVertActive;
	float			speedVertLazy;
//	float			speedVertMean2S;
//	float			speedVertStat;

    FloatFIFO       speedVertHistory;
    /*
	float			speedVertHistory[MAX_VARIO_HISTORY]; // 1s vertial speed history
	uint16_t		historyFront;
	uint16_t		historyRear;
    */
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

	//
	float			pressure;
	float			pressureLazy;
	float			temperature;		// by barometer
//	float			temperatureAlt;		// by thermometer

	//
	time_t			timeCurrent;
//	time_t			timeStart;
//	time_t			timeFly;
};


struct FlightState
{
	//
	uint32_t		takeoffTime;    // takeOffTime
	Position		takeoffPos;     // takeOffPos

	uint32_t		flightTime;

	//
	uint32_t		circlingStartTime;
	Position		circlingStartPos; // latitude, longitude, altitude
	uint32_t		circlingTime;
	int32_t			circlingGain;
    int32_t			circlingInclineAzimuth;
	int32_t			circlingIncline; // tilt, slope ??

	int16_t			deltaHeading_AVG;
	int16_t			deltaHeading_SUM;

	//
    TrackHistory    trackHistory;
    /*
	TrackPoint		trackPoints[MAX_TRACK_HISTORY];
	TrackDistance	trackDistance[MAX_TRACK_HISTORY];
//	TrackHistory	trackHistory[MAX_TRACK_HISTORY];
	int16_t			frontPoint;
	int16_t			rearPoint;
    */

	//
	Position		glidingStartPos;
	int32_t			glidingCount;
	float			glidingRatio; // glideRatio // L/D(Lift to Drag)

	//
	float			distTakeoff;
	float			distFlightTerm; // distFlight
	float			distFlightTotal; // distFlightAccum

	float			distTarget;
	float			distNextTarget;

	//
	int16_t			bearingTakeoff;
	int16_t			bearingTarget;
	int16_t			bearingNextTarget;

	//
	FlightMode		flightMode;

	XcTaskPtr		xcTaskPtr;
};


struct FlightStats
{
	float			altitudeMax;
	float			altitudeMin;

	float			varioMax;
	float			varioMin;

    float           varioAvgSink;       // avgSink
    float           varioAvgClimb;      // avgClimb


	int16_t			thermalingCount;    // totalThermaling;
	int16_t			thermalingGainMax;  // thermalingMaxGain
};


////////////////////////////////////////////////////////////////////////////////
// DeviceContext

struct DeviceContext
{
    DeviceSettings      deviceSettings;
    UserSettings        userSettings;
    VarioSettings       varioSettings;

    DeviceState         deviceState;
    VarioState          varioState;
    FlightState         flightState;

    FlightStats         flightStats;

    /*
    GliderInfo          gliderInfo;
    IGCLoggerData       logger;

//  VarioTone           toneTable[MAX_VARIO_TONE];

    VolumeSettings      volume;
    ThresholdSettings   threshold;

//  KalmanParameters    kalman;

    DeviceDefault       deviceDefault;
    */
};



// Setting Items
//

//
//
// sealevel-pressure
// 
//
//
// vario-threshold: sink, climb, sensitivity(dead-zone), damplingFactor(?)
// bluetooth-vario-sentence: LK8, NxNav
// reference-altitude: 1, 2, 3
// 
// glider: type, manufacture, model
//
//   //switch: igc-logging, nmea-logging, bluetooth, sound, auto-shutdown
//
// logging-threshold: takeoff-speed, landing-speed, landing-timeout, logging-interval
// logging-info: pilot, ....
//
//
// vario-tone-table
//
// sound: off, sound-on-when-power-on, sound-on-when-launch : off, on, auto
// 
// 
// device: bluetooth-switch, sound-switch, simulation-switch, nmea-logging-switch, igc-logging-switch
//         wifi-ssid, wifi-password, bluetooth-name, 
//         timezone
//
//
// display: compass-up: north, heading, target
//
//
// last: page, sub-page
//
















#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif // __DEVICE_CONTEXT_H__
