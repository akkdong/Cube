// Route.h
//

#ifndef __XC_ROUTE_H__
#define __XC_ROUTE_H__

#include <iostream>

#include "WayPoints.h"
#include "ArduinoJson.h"



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

	void addTurnPoint(XcTurnPointPtr pointPtr) { points.push_back(pointPtr); }
	void reset() { points.clear(); }


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

	//
	void setTaskType(TaskType type) { taskType = type; }
	void setGateOpen(time_t t) { gateOpen = t; }
	void setDeadLine(time_t t) { deadLine = t; }
	void setEarthModel(EarthModel model) { earthModel = model; }

	void addTurnPoint(XcTurnPointPtr pointPtr) { route.addTurnPoint(pointPtr); }
	void resetTurnPoint() { route.reset(); }

	//
	bool load(std::istream &in);
	/*
	bool load(fs::File& f);
	bool load(Stream &s);
	*/

protected:
	void set(JsonDocument &doc);

protected:
	int	version;
	TaskType taskType;
	time_t gateOpen;
	time_t deadLine;
	EarthModel earthModel;
	XcRoute route;
};


#endif // __XC_ROUTE_H__
