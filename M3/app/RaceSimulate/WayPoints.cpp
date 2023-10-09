// WayPoints.cpp
//

#include "pch.h"
#include "WayPoints.h"




///////////////////////////////////////////////////////////////////////////////
// class XcPoint

XcPoint::XcPoint()
{
}

XcPoint::XcPoint(const char *_name, float _lat, float _lon, float _alt, const char *_desc)
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

void XcPoint::setLatitude(float lat)
{
	this->lat = lat;
}

void XcPoint::setLongitude(float lon)
{
	this->lon = lon;
}

void XcPoint::setAltitude(float alt)
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
