// Variometer.h
//

#ifndef __VARIOMETER_H__
#define __VARIOMETER_H__

#include "abstract/Barometer.h"
#include "abstract/VarioFilter.h"


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
	float					getAltitude() { return altitude; }
	float					getAltitudeFiltered() { return altitudeFiltered; }
	float					getAltitudeCalibrated() { return altitudeFiltered + altitudeDrift; }
	float					getVelocity() { return vario; }

	void					calibrateAltitude(float altitudeRef) { altitudeDrift = altitudeRef - altitudeFiltered; }
	void					calibrateSeaLevel(float altitudeRef);
	
protected:
	int						measure();
	float					calculateSeaLevel(float pressure, float temperature, float altitude);

protected:
	// sensor
    IBarometer *            baroSensor;
    // filter
    IVarioFilter*           varioFilter;

    // vario
	int						updateCount;
    
	float					pressure;
	float					temperature;
	float					seaLevel;
	float					altitudeDrift;
	float					altitude;
    float                   altitudeFiltered;

    float                   vario;
};


#endif // __VARIOMETER_H__
