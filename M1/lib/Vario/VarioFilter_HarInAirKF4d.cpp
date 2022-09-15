// VarioFilter_HarInAirKF4d.cpp
//

#include <stdint.h>
#include <math.h>

#include "VarioFilter_HarInAirKF4d.h"
#include "logger.h"


#ifndef KALMAN_UPDATE_FREQ
#define KALMAN_UPDATE_FREQ          (25)
#endif

// altitude noise variance can be measured offline by calculating the 
// statistical variance in cm^2 of altitude samples from 
// the baro sensor at rest
#ifndef KF_Z_MEAS_VARIANCE
#define KF_Z_MEAS_VARIANCE          (200)
#endif

// KF4 Acceleration Measurement Noise variance
#ifndef KF_A_MEAS_VARIANCE
#define KF_A_MEAS_VARIANCE   		50.0f
#endif

// This is set low as the residual acceleration bias after calibration
// is expected to have little variation/drift
#ifndef KF_ACCELBIAS_VARIANCE
#define KF_ACCELBIAS_VARIANCE       0.005f
#endif


///////////////////////////////////////////////////////////////////////////////////////////////
//

VarioFilter_HarInAirKF4d::VarioFilter_HarInAirKF4d()
{
    // ...
}


// Tracks the position z, and velocity v of an object moving in a straight line,
// that is perturbed by random accelerations. Assumes we have sensors providing
// periodic measurements for acceleration a and position z. 
// aVariance should be specified with a large enough value to allow the true state (z, v) to be within the uncertainty estimate.
// It should be set higher for more thermic/turbulent conditions
// kAdapt injects more uncertainty in high acceleration situations to allow the filter to respond quicker.
// zInitial can be determined by averaging a few samples of the altitude measurement.
// vInitial and aInitial can be set to zero.

void VarioFilter_HarInAirKF4d::begin(float aVariance,  float kAdapt, float zInitial, float vInitial, float aInitial)
{
	ZMeasVariance = KF_Z_MEAS_VARIANCE;
	AMeasVariance = KF_A_MEAS_VARIANCE;
    ABiasVariance = KF_ACCELBIAS_VARIANCE;
	AccelVariance = aVariance;
	KAdapt = kAdapt;

	State.z = zInitial;
	State.v = vInitial;
    State.a = aInitial;
	State.b = 0.0f; // assume residual acceleration bias = 0 initially

	Pzz = 1500.0f;
    Pzv = 0.0f;
	Pza = 0.0f;
	Pzb = 0.0f;
	
	Pvz = Pzv; 
	Pvv = 1500.0f;
	Pva = 0.0f;
	Pvb = 0.0f;
	
	Paz = Pza;
	Pav = Pva;
	Paa = 100000.0f;
	Pab = 0.0f;

	Pbz = Pzb;
	Pbv = Pvb;
	Pba = Pab;
	Pbb = 1500.0f;
}


// Process model state transition matrix F  (4x4)
//  | 1   dt  dt^2/2 -dt^2/2 |
//  | 0   1   dt     -dt     |
//  | 0   0   1       0      |
//  | 0   0   0       1      |
//
void VarioFilter_HarInAirKF4d::predict(float dt) 
{
	// Predicted (a priori) state vector estimate x_k- = F * x_k-1+
	float accel_true = State.a - State.b; // true acceleration = acceleration minus acceleration sensor bias
	State.z = State.z + (State.v * dt) + (accel_true * dt * dt* 0.5f);
	State.v = State.v + (accel_true * dt);

	// Predicted (a priori) state covariance estimate P_k- = (F * P_k-1+ * F_t) + Qk
	float dt2 = dt*dt;  // dt^2
	float dt3 = dt2*dt; // dt^3 
	float dt4 = dt2*dt2; // dt^4;
	float dt2div2 = dt2*0.5f; // dt^2/2
	float dt3div2 = dt3*0.5f; // dt^3/2
	float dt4div2 = dt4*0.5f; // dt^4/2
	float dt4div4 = dt4*0.25f; // dt^4/4
	
	float p00 = Pzz + 2.0f*Pzv*dt + (Pza - Pzb)*dt2  + Pvv*dt2div2 + (Pva - Pvb)*dt3 + (Paa+Pbb)*dt4div4 - Pab*dt4div2;
	float p01 = Pzv + dt*(Pza - Pzb + Pvv) + 3.0f*dt2div2*(Pva - Pvb) - Pab*dt3 + (Paa + Pbb)*dt3div2;
	float p02 = Pza + Pva*dt + (Paa - Pba)*dt2div2;
	float p03 = Pzb + Pvb*dt + (Pab - Pbb)*dt2div2;

	float p11 = Pvv + 2.0f*dt*(Pva - Pvb) + dt2*(Paa - 2.0f*Pab + Pbb);
	float p12 = Pva + dt*(Paa - Pba);
	float p13 = Pvb + dt*(Pab - Pbb);

	float p22 = Paa;
	float p23 = Pab;
	float p33 = Pbb;

	Pzz = p00;
	Pzv = p01;
	Pza = p02;
	Pzb = p03;

	Pvz = Pzv;
	Pvv = p11;
	Pva = p12;
	Pvb = p13;

	Paz = Pza;
	Pav = Pva;
	Paa = p22;
	Pab = p23;

	Pbz = Pzb;
	Pbv = Pvb;
	Pba = Pab;
	Pbb = p33; 

	// Add Q_k
	Paa = Paa + AccelVariance;
	Pbb = Pbb + ABiasVariance;
}

// Update state estimate and state covariance estimate, given new z and a measurements
// Measurement vector m_k
// m_k = | zm_k |
//       | am_k |
// H matrix transforms state vector space to measurement space 
// | 1 0 0 0 |
// | 0 0 1 0 |
// Predicted measurement from a_priori state estimate = (H * x_k-)
// Innovation error y_k = actual measurement m_k minus predicted measurement
// y_k = m_k - (H * x_k-)
// 2x2 sensor noise covariance matrix R_k
// | zsensor_variance  0                |  
// | 0                 asensor_variance |

void VarioFilter_HarInAirKF4d::update(float zm, float am, float* pz, float* pv) 
{
	// Innovation Error y_k = measurement minus apriori estimate
	float z_err = zm - State.z;
	float a_err = am - State.a;

	// Innovation covariance S_k
	// S_k = (H * P_k- * H_t) + R_k
	float s00 = Pzz;
	float s01 = Pza;
	float s10 = s01;
	float s11 = Paa;

	// add R_k
	s00 += ZMeasVariance;
	s11 += AMeasVariance;
	// inject additional uncertainty for high acceleration situations
	float accel_ext = abs(am-State.b);
	s11 += (KAdapt*accel_ext*accel_ext);

	// allow system to update acceleration sensor bias estimate only when there is low acceleration
	ABiasVariance = KF_ACCELBIAS_VARIANCE/(1.0f + accel_ext);	

	// Compute S_k_inv
	float sdetinv = 1.0f/(s00*s11 - s10*s01);
	float sinv00 = sdetinv * s11;
	float sinv01 = -sdetinv * s10;
	float sinv10 = sinv01;
	float sinv11 = sdetinv * s00;

	// Kalman gain K_k [4x2] matrix
	// K_k = P_k- * H_t * S_k_inv
	float k00 = Pzz*sinv00 + Pza*sinv10;  
	float k01 = Pzz*sinv01 + Pza*sinv11;
	float k10 = Pvz*sinv00 + Pva*sinv10;
	float k11 = Pvz*sinv01 + Pva*sinv11;
	float k20 = Paz*sinv00 + Paa*sinv10;
	float k21 = Paz*sinv01 + Paa*sinv11;
	float k30 = Pbz*sinv00 + Pba*sinv10;
	float k31 = Pbz*sinv01 + Pba*sinv11;

	// Updated (a posteriori) state estimate x_k+ 
	// x_k+ = x_k- + K_k * y_k
	State.z = State.z + (k00*z_err + k01*a_err);
	State.v = State.v + (k10*z_err + k11*a_err);
	State.a = State.a + (k20*z_err + k21*a_err);
	State.b = State.b + (k30*z_err + k31*a_err);

	// Updated (a posteriori) state covariance estimate P_k+
	// P_k+ = (I - K_k * H_k)*P_k-
	float tmp = 1.0f - k00;
	float p00 = tmp*Pzz - k01*Paz;
	float p01 = tmp*Pzv - k01*Pav;
	float p02 = tmp*Pza - k01*Paa;
	float p03 = tmp*Pzb - k01*Pab;

	float p11 = -k10*Pzv + Pvv - k11*Pav;
	float p12 = -k10*Pza + Pva - k11*Paa;
	float p13 = -k10*Pzb + Pvb - k11*Pab;

	float p22 = -k20*Pza + (1.0f-k21)*Paa;
	float p23 = -k20*Pzb + (1.0f-k21)*Pab;

	float p33 = -k30*Pzb -k31*Pab + Pbb;

	Pzz = p00;
	Pzv = p01;
	Pza = p02;
	Pzb = p03;

	Pvz = Pzv;
	Pvv = p11;
	Pva = p12;
	Pvb = p13;

	Paz = Pza;
	Pav = Pva;
	Paa = p22;
	Pab = p23;

	Pbz = Pzb;
	Pbv = Pvb;
	Pba = Pab;
	Pbb = p33; 

	// return the state variables of interest (z and v)
	*pz = State.z;
	*pv = State.v;

    //
	LOGd("%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",zm, State.z, Pzz, State.v, Pvv, State.a - State.b, Paa, State.b, Pbb);
}

void VarioFilter_HarInAirKF4d::reset(float altitude)
{
}

