// Route.cpp
//

#include "pch.h"
#include "Route.h"



///////////////////////////////////////////////////////////////////////////////
// class XcTurnPoint

XcTurnPoint::XcTurnPoint()
{
}

XcTurnPoint::XcTurnPoint(Type _type, float _r, const char *name, float lat, float lon, float alt, const char *desc)
	: XcPoint(name, lat, lon, alt, desc)
	, type(_type)
	, radius(_r)
{

}

XcTurnPoint::XcTurnPoint(Type _type, float _r, XcPoint& point)
	: XcPoint(point)
	, type(_type)
	, radius(_r)
{

}


///////////////////////////////////////////////////////////////////////////////
// class XcRoute

XcRoute::XcRoute()
{
}


///////////////////////////////////////////////////////////////////////////////
// class XcTask

XcTask::XcTask()
{
}
