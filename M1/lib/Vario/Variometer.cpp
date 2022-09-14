// Variometer.cpp
//

#include "Variometer.h"



///////////////////////////////////////////////////////////////////////////////////////////////
// class Variometer

Variometer::Variometer() : baroSensor(nullptr), varioFilter(nullptr)
{
}

int Variometer::begin(IBarometer* baro, IVarioFilter* filter)
{
    baroSensor = baro;
    varioFilter = filter;

    return 0;
}

void Variometer::end()
{
}

int Variometer::update()
{
    return 0; // updated > 0
}

void Variometer::resetUpdate()
{
}

void Variometer::calibrateAltitude(float altitudeRef)
{
}

void Variometer::calculateSeaLevel(float altitude)
{
}

void Variometer::measure()
{
}
