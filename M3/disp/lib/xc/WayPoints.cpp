// WayPoints.cpp
//

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "WayPoints.h"




///////////////////////////////////////////////////////////////////////////////
// class XcBasePoint

XcBasePoint::XcBasePoint()
	: lat(0)
	, lon(0)
{
}

XcBasePoint::XcBasePoint(Math::real _lat, Math::real _lon)
	: lat(_lat)
	, lon(_lon)
{
}



///////////////////////////////////////////////////////////////////////////////
// class XcPoint

XcPoint::XcPoint()
	: XcBasePoint()
	, name("")
	, description("")
	, altitude(0)
{
}

XcPoint::XcPoint(const char *_name, Math::real _lat, Math::real _lon, Math::real _alt, const char *_desc)
	: XcBasePoint(_lat, _lon)
	, name(_name)
	, description(_desc)
	, altitude(_alt)
{
}

void XcPoint::setName(const char *name)
{
	this->name = name;
}

void XcPoint::setDescription(const char *desc)
{
	this->description = desc;
}

void XcPoint::setAltitude(Math::real alt)
{
	this->altitude = alt;
}



///////////////////////////////////////////////////////////////////////////////
// class XcWayPoints

XcWayPoints::XcWayPoints()
	: version(1)
	, unit(METER)
{

}
