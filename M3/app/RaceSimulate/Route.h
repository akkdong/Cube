// Route.h
//

#ifndef __XC_ROUTE_H__
#define __XC_ROUTE_H__

#include "WayPoints.h"


///////////////////////////////////////////////////////////////////////////////
// class XcTurnPoint

class XcTurnPoint : public XcPoint
{
public:
	enum Type {
		TURN,
		TAKEOFF,
		SSS,
		ESS,
		GOAL,
	};

	XcTurnPoint();
	XcTurnPoint(Type type, float radius, const char *name, float lat, float lon, float alt, const char *desc = nullptr);
	XcTurnPoint(Type type, float radius, XcPoint& point);

	Type getType() { return type; }
	float getRadius() { return radius; }

protected:
    float radius;
	Type type;
};

typedef std::shared_ptr<XcTurnPoint> XcTurnPointPtr;


///////////////////////////////////////////////////////////////////////////////
// class XcRoute

class XcRoute
{
public:
    XcRoute();

	void addTurnPoint(XcTurnPointPtr pointPtr);
	void reset();


protected:
    std::vector<XcTurnPointPtr> points;
};



///////////////////////////////////////////////////////////////////////////////
// class XcTask

class XcTask
{
public:
    XcTask();

	enum TaskType {
		CLASSIC,
		COMPETITION,
	};

	enum EarthModel {
		FAISPHERE,
		WGS84,
	};

protected:
	int	version;
	TaskType taskType;
	time_t	gateOpen;
	time_t	deadLine;
	EarthModel	earthModel;
	XcRoute route;
};


#endif // __XC_ROUTE_H__
