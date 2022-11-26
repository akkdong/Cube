// app_defines.h
//

#ifndef __APP_DEFINES_H__
#define __APP_DEFINES_H__


// Variometer default settings
//

#define VARIOMETER_TIME_ZONE					(9)	// GMT+9
#define VARIOMETER_BEEP_VOLUME					(100)	// percentage (0 ~ 100)
#define VARIOMETER_EFFECT_VOLUME				(100)

#define VARIOMETER_SINKING_THRESHOLD 			(-3.0)
#define VARIOMETER_CLIMBING_THRESHOLD 			(0.2)
#define VARIOMETER_SENSITIVITY 					(0.12)
#define VARIOMETER_MIN_VELOCITY					(-10)
#define VARIOMETER_MAX_VELOCITY					(10)

#define VARIOMETER_LXNAV_SENTENCE				(1)
#define VARIOMETER_LK8_SENTENCE					(2)
#define VARIOMETER_DEFAULT_NMEA_SENTENCE		VARIOMETER_LK8_SENTENCE

#define VARIOMETER_SENTENCE_DELAY				(1000)

#define VARIOMETER_DEFAULT_DAMPING_FACTOR 		(0.05)	// x1 = x0 + (x1 - x0) * damping_factor


// Flight detection settings
//

#define FLIGHT_START_MIN_SPEED 					(6.0)		// 6Km/h
#define FLIGHT_STOP_MIN_SPEED					(2.0)
#define FLIGHT_LANDING_THRESHOLD 				(10000)		// 10s
#define FLIGHT_LOGGING_INTERVAL					(1000)		// 1s

// KalmanFilter class configuration
#define KF_ZMEAS_VARIANCE       				(400.0f)
#define KF_ZACCEL_VARIANCE      				(1000.0f)
#define KF_ACCELBIAS_VARIANCE   				(1.0f)

// Auto shutdown threshold
//

#define STABLE_SINKING_THRESHOLD				(-0.2)
#define STABLE_CLIMBING_THRESHOLD				(0.2)
#define AUTO_SHUTDOWN_THRESHOLD					(10*60*1000)	// 10min (600s)

// Low battery threshold
//

#define LOW_BATTERY_THRESHOLD					(2.8)	// VBAT drop down about 0.4v. so rear threshold voltage is -0.4
#define SHUTDOWN_HOLD_TIME						(2000)



#endif // __APP_DEFINES_H__
