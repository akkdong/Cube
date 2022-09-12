// lv_app.h
//

#ifndef __LV_APP_H__
#define __LV_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#define MAX_STRING_SIZE					(16)
#define MAX_VARIO_HISTORY				(30)
#define MAX_VARIO_TONE					(12)

#define MAX_TRACK_HISTORY				(30)

typedef struct app_conf
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
	float			speedVertSumTotal;
	int16_t			speedVertSumCount;
	int16_t			speedVertNext;

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
} app_conf_t;



void app_config_init(app_conf_t* conf);
void app_init();

#ifdef __cplusplus
}
#endif

#endif // __LV_APP_H__
