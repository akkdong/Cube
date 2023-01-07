// Variometer.h
//

#ifndef __VARIOMETER_H__
#define __VARIOMETER_H__

#include <stdint.h>

#include "abstract/Barometer.h"
#include "abstract/VarioFilter.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// class Variometer

class Variometer
{
public:
	Variometer();

public:
	virtual int				begin(IBarometer* baro, IVarioFilter* filter);
	virtual void			end();

	virtual int				update();
	virtual int				updatePeriodic();
	
	virtual void			resetUpdate();

	float					getPressure() { return pressure; }
	float					getTemperature() { return temperature; }
	float					getAltitudeFiltered() { return altitudeFiltered; }
	float					getAltitudeCalibrated() { return altitudeFiltered + altitudeDrift; }
	float					getAltitude() { return altitude; }
	float					getVelocity() { return vario; }

	void					calibrateAltitude(float altitudeRef) { altitudeDrift = altitudeRef - altitudeFiltered; }
	void					calibrateSeaLevel(float altitudeRef);
	
protected:
	int						measure();
	void					updateInternal();

	float					calculateSeaLevel(float pressure, float temperature, float altitude);

protected:
	// sensor
    IBarometer *            baroSensor;
    // filter
    IVarioFilter*           varioFilter;

    // vario
	int						updateCount;
	uint32_t				lastUpdateTick;
    
	float					pressure;
	float					temperature;
	float					seaLevel;
	float					altitudeDrift;
	float					altitude;
    float                   altitudeFiltered;
	
    float                   vario;
};


#endif // __VARIOMETER_H__
