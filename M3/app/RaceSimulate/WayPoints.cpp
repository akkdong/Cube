// WayPoints.cpp
//

#include "WayPoints.h"




///////////////////////////////////////////////////////////////////////////////
// class XcPoint

XcPoint::XcPoint()
	: name("")
	, description("")
	, lat(0)
	, lon(0)
	, altitude(0)
{
}

XcPoint::XcPoint(const char *_name, double _lat, double _lon, double _alt, const char *_desc)
	: name(_name)
	, description(_desc)
	, lat(_lat)
	, lon(_lon)
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

void XcPoint::setLatitude(double lat)
{
	this->lat = lat;
}

void XcPoint::setLongitude(double lon)
{
	this->lon = lon;
}

void XcPoint::setAltitude(double alt)
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
