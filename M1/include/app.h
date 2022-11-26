// app.h
//

#ifndef __LV_APP_H__
#define __LV_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#define MAX_STRING_SIZE					(16)
#define MAX_VARIO_HISTORY				(120)
#define MAX_VARIO_TONE					(12)

#define MAX_TRACK_HISTORY				(120)




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
	int16_t			speedVertCount;
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
};

struct FlightStats
{
	float			altitudeMax;
	float			altitudeMin;

	float			varioMax;
	float			varioMin;

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
	float			lat;
	float			lon;
	float			vario;
};

struct TrackDistance
{
	float			dx;
	float			dy;
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

struct DeviceContextData
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



class DeviceContext : public DeviceContextData
{
public:
	DeviceContext() {}

public:
	//
	void updateVSpeedHistory(float speed) {}
	void updateTrackHistory(float lat, float lon, float speedVert) {}
};




#if 0
typedef struct app_conf
{
	//
	int				dirty;

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
	float			glideRatio;	
	
	//
	float			speedGround;
	float			speedAir;
	float			speedVertActive;
	float			speedVertLazy;
//	float			speedVertMean2S;
//	float			speedVertStat;

	float			speedVertHistory[MAX_VARIO_HISTORY]; // 1s vertial speed history
	float			speedVertSumTotal;
	int32_t			speedVertSumCount;
	int32_t			speedVertNext;

	//
	float			longitude;
	float			latitude;

	float			longitudeLast;
	float			latitudeLast;

	float			longitudeStart;	// take-off longitude
	float			latitudeStart;	// take-off latitude

	float			distTakeoff;	// distance from take-off place
	float			distFlight;		// total flight distance(odometer?)

	int16_t			heading;		// angle(radian)
	int16_t			headingLast;
	int16_t			bearing;
	
	//
	float			pressure;
	float			temperature;		// by barometer
	float			temperatureAlt;		// by thermometer

	//
	time_t			timeCurrent;
	time_t			timeTakeoff;
	time_t			timeFly;
} app_conf_t;

typedef struct vario_conf
{
	float			sinkThreshold;
	float			climbThreshold;
	float			sensitivity;
	
	uint8_t			sentence;			// 
	
	float			altitudeRef1;		// reference base altitude
	float			altitudeRef2;
	float			altitudeRef3;
	
	float			dampingFactor; 		//  affect to VarioState.speedVertLazy			

	//
	uint8_t			gliderType;
	char			gliderManufacture[MAX_STRING_SIZE];
	char			gliderModel[MAX_STRING_SIZE];	
	char			pilotName[MAX_STRING_SIZE];

	//
	uint8_t			enableIGCLogging; // igc
	
	int				takeoffSpeed;
	int				landingSpeed;
	int				landingTimeout;
	int				loggingInterval;
	
	float			timezone;

	// volume
	uint8_t			volumeMax;
	uint8_t			volumeMid;
	uint8_t			volumeMin;

	uint8_t			volumeCur;

	uint8_t			volumeAutoTurnon;

	//
	float			lowBattery;
	uint32_t		shutdonwHoldTime;
	uint32_t		autoShutdonwVario;

	//
	float			kalmanZMeas;
	float			kalmanZAccel;
	float			kalmanAccelBias;

	//
	//uint8_t			enableBT;
	//uint8_t			enableSound; 

	char			btName[MAX_STRING_SIZE];

	uint8_t			enableSimulation;
	uint8_t			enableNmeaLogging;

	char			wifiSSID[MAX_STRING_SIZE];
	char			wifiPassword[MAX_STRING_SIZE];

} vario_conf_t; // or device_conf_t

typedef struct vario_status
{
	float			batteryPower;
	uint8_t			deviceStatus; // BT, GPS, SD
} vario_status_t;


typedef struct flight_stat
{
	float			altitudeMax;
	float			altitudeMin;

	float			varioMax;
	float			varioMin;

	int16_t			totalThermaling;
	int16_t			thermalingMaxGain;

} flight_stat_t;

typedef struct position
{
	float			lat;
	float 			lon;
	float			alt;

} position_t;

typedef struct track_point
{
	float			lat;
	float			lon;
	float			vario;
} track_point_t;

typedef struct track_distance
{
	float			dx;
	float			dy;
} track_distance_t;

typedef struct flight_state
{
	enum Mode
	{
		FMODE_READY,	// READY, LANDING
		FMODE_FLYING,
		FMODE_CIRCLING,
		FMODE_GLIDING,
	};	

	//
	uint32_t		takeOffTime;
	position_t		takeOffPos;
	uint32_t		flightTime;

	//
	uint32_t		circlingStartTime;
	position_t		circlingStartPos; // latitude, longitude, altitude
	uint32_t		circlingTime;
	int32_t			circlingGain;
	int32_t			circlingIncline; // tilt, slope ??

	int16_t			deltaHeading_AVG;
	int16_t			deltaHeading_SUM;

	//
	track_point_t	trackPoints[MAX_TRACK_HISTORY];
	track_distance_t trackDistance[MAX_TRACK_HISTORY];
	int16_t			frontPoint;
	int16_t			rearPoint;

	//
	position_t		glidingStartPos;
	int32_t			glidingCount;
	float			glideRatio; // L/D(Lift to Drag)

	//
	int16_t			bearingTakeoff;
	int16_t			bearingNextPoint;

	//
	float			distTakeoff;
	float			distFlight;
	float			distNextPoint;

	//
	Mode			flightMode;

} flight_state_t;



////////////////////////////////////////////////////////////////////////////
//

void 			app_config_init(app_conf_t* conf);
void 			app_init();
void 			app_update();

app_conf_t* 	app_get_conf();

#endif

#ifdef __cplusplus
}
#endif

#endif // __LV_APP_H__
