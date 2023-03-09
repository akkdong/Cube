#include <Arduino.h>
#include "bsp.h"
#include "logger.h"

#include "Variometer.h"
#include "keypad.h"
#include "Beeper.h"


#define VFILTER_HARINAIR_KF2     1
#define VFILTER_HARINAIR_KF3     2
#define VFILTER_HARINAIR_KF4d    3
#define VFILTER_ROBIN_KF         4

#define USE_KALMAN_FILTER        VFILTER_HARINAIR_KF3

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
#include "VarioFilter_HarInAirKF2.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
#include "VarioFilter_HarInAirKF3.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
#include "VarioFilter_HarInAirKF4d.h"
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
#include "VarioFilter_RobinKF.h"
#else
#error "Invalid vario kalman-filter"
#endif

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
VarioFilter_HarInAirKF2     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
VarioFilter_HarInAirKF3     varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
VarioFilter_HarInAirKF4d    varioFilter;
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
VarioFilter_RobinKF         varioFilter;
#endif


class KeypadHandler : public IKeypadCallback
{
public:
    virtual void onPressed(uint8_t key) {
      LOGi("key-pressed: %d", key);
    }
    virtual void onLongPressed(uint8_t key) {
      LOGi("key-long-pressed: %d", key);
    }
    virtual void onReleased(uint8_t key) {
      LOGi("key-released: %d", key);
    }
};

Variometer vario;
KeypadHandler keyHandler;
Keypad keyPad;
Beeper beeper;


void setup() 
{
  bsp_hal_init();

  #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
  varioFilter.begin(40.0f, 1000000.0f, 0);
  #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
  varioFilter.begin(500.0f, 6000.0f, 1.0f, 0);
  #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d

  // injects additional uncertainty depending on magnitude of acceleration
  // helps respond quickly to large accelerations while heavily filtering
  // in low acceleration situations.  Range : 0.5 - 1.5
  #define KF_ACCEL_VARIANCE_DEFAULT   100     // 50 ~ 150
  #define KF_ADAPT_DEFAULT            10     // 50 ~ 150

  varioFilter.begin(KF_ACCEL_VARIANCE_DEFAULT * 1000.0f, KF_ADAPT_DEFAULT / 100.0f, 0, 0, 0);
  #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
  varioFilter.Configure(30.0f, 4.0f, altitude);
  #endif   

  vario.begin(CreateBarometer(), &varioFilter);
  keyPad.begin(CreateKeypadInput(), &keyHandler);
  beeper.begin(CreateTonePlayer());

  // outputs a sinus signal with frequency ~1000Hz and max. amplitude (default)
  //dac1.outputCW(1000);
  // outputs a sinus signal with frequency ~1000Hz, max. amplitude (default) and 180° phaseshift
  //dac2.outputCW(1000, DAC_CW_SCALE_1, DAC_CW_PHASE_180);  
}

void loop() 
{
  uint8_t lastChar = 0;
  while (SerialGPS.available())
  {
    lastChar = SerialGPS.read();
    SerialHost.write(lastChar);
    //SerialDebug.write(lastChar);

    if (lastChar == '\n')
      break;
  }

  keyPad.update();
  beeper.update();

  static float altitude = 0.0f, vspeed = 0.0f;
  if (vario.update() > 0)
  {
    // update vertical-speed, barometric-altitude, temperature
    float alt = vario.getAltitudeFiltered();
    float vel = vario.getVelocity();

    altitude = altitude + (alt - altitude) * 0.1f;
    vspeed = vspeed + (vel - vspeed) * 0.1f;

    // update beep, vario-nmea, ...
  }

  static uint32_t lastTick = millis();
  if (millis() - lastTick > 1000)
  {
    LOGv("Altitude: %f, VSpeed: %f\r\n", altitude, vspeed);
    lastTick = millis();

    beeper.setVelocity(vspeed);
  }
}
