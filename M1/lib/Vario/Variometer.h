// Variometer.h
//

#ifndef __VARIOMETER_H__
#define __VARIOMETER_H__

#include "Barometer.h"
#include "VarioFilter.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// class Variometer

class Variometer
{
public:
	Variometer();

public:
	int						begin(IBarometer* baro, IVarioFilter* filter);
	void					end();

	int						update();
	void					resetUpdate();

	float					getPressure() { return pressure; }
	float					getTemperature() { return temperature; }
	float					getAltitudeFiltered() { return altitudeFiltered; }
	float					getAltitude() { return altitude; }
	float					getVelocity() { return vario; }

	void					calibrateAltitude(float altitudeRef);
	void					calculateSeaLevel(float altitude);
	
protected:
	  void					measure();

protected:
	// sensor
    IBarometer *            baroSensor;
    // filter
    IVarioFilter*           varioFilter;

    // vario
	int						updateCount;
    
	float					seaLevel;
	float					pressure;
	float					temperature;
	float					altitude;

    float                   altitudeFiltered;
    float                   vario;
};


#endif // __VARIOMETER_H__
