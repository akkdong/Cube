// config.h
//

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifndef PI
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif


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

#define VARIOMETER_SENTENCE_DELAY				(1000)  //  tick = 1000/25 = 40, 25 tick = 1000
#define VARIOMETER_DEFAULT_DAMPING_FACTOR 		(0.05)	// x1 = x0 + (x1 - x0) * damping_factor

#define SENSOR_UPDATE_FREQUENCY                 (25)    // 25Hz


// Kalman filter settings
//

// Variometer class configuration
#define POSITION_MEASURE_STANDARD_DEVIATION 	(0.1)
#define ACCELERATION_MEASURE_STANDARD_DEVIATION (0.3)

// KalmanFilter class configuration
#define KF_ZMEAS_VARIANCE       				(500.0f)
#define KF_ZACCEL_VARIANCE      				(8000.0f)
#define KF_ACCELBIAS_VARIANCE   				(1.0f)


// Flight detection settings
//

#define FLIGHT_START_MIN_SPEED 					(6.0)		// 6Km/h
#define FLIGHT_STOP_MIN_SPEED					(2.0)
#define FLIGHT_LANDING_THRESHOLD 				(10000)		// 10s
#define FLIGHT_LOGGING_INTERVAL					(1000)		// 1s


// Auto shutdown threshold
//

#define STABLE_SINKING_THRESHOLD				(-0.2)
#define STABLE_CLIMBING_THRESHOLD				(0.2)
#define AUTO_SHUTDOWN_THRESHOLD					(10*60*1000)	// 10min (600s)

// Low battery threshold
//

#define LOW_BATTERY_THRESHOLD					(3.2)	// VBAT drop down about 0.4v. so rear threshold voltage is -0.4
#define SHUTDOWN_HOLD_TIME						(2000)

#define KEY_ENTER           (0xB0)
#define KEY_ESCAPE          (0xB1)
#define KEY_LEFT            (0xD8)
#define KEY_RIGHT           (0xD7)


#endif // __CONFIG_H__