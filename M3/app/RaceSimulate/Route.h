// Route.h
//

#ifndef __XC_ROUTE_H__
#define __XC_ROUTE_H__

#include <iostream>

#include "WayPoints.h"
#include "ArduinoJson.h"

class XcTurnPoint;
class XcRoute;
class XcTask;


///////////////////////////////////////////////////////////////////////////////
// class XcTurnPoint

class XcTurnPoint : public XcPoint
{
	friend class XcRoute;
	friend class XcTask;

public:
	enum Type {
		TURN,
		TAKEOFF,
		SSS,
		ESS,
		GOAL,
	};

	XcTurnPoint();
	XcTurnPoint(Type type, double radius, const char *name, double lat, double lon, double alt, const char *desc = nullptr);
	XcTurnPoint(Type type, double radius, XcPoint& point);

	Type getType() { return type; }
	double getRadius() { return radius; }
	double getTheta() { return theta; }

protected:
	Type type;
	double radius;
	double theta;
};

typedef std::shared_ptr<XcTurnPoint> XcTurnPointPtr;


///////////////////////////////////////////////////////////////////////////////
// class XcRoute

class XcRoute
{
	friend class XcTask;

public:
    XcRoute();

	XcTurnPointPtr getTurnPoint(size_t index);
	size_t getTurnPointCount() { return points.size(); }
	double getTotalDistance() { return totalDist; }
	double getOptimizedDistance() { return optimizedDist; }

	void addTurnPoint(XcTurnPointPtr pointPtr) { points.push_back(pointPtr); }
	void reset();

	void optimize(size_t startPoint, double lat, double lon);
	void optimize(size_t startPoint = 0);

	void calcTotalDistance();

protected:
	

protected:
    std::vector<XcTurnPointPtr> points;

	// 
	double totalDist;
	double optimizedDist;
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
	XcRoute &getRoute() { return route; }
	TaskType getTaskType() { return taskType; }
	time_t getGateOpen() { return gateOpen; }
	time_t getDeadLine() { return deadLine; }
	EarthModel getEatchModel() { return earthModel; }

	void setTaskType(TaskType type) { taskType = type; }
	void setGateOpen(time_t t) { gateOpen = t; }
	void setDeadLine(time_t t) { deadLine = t; }
	void setEarthModel(EarthModel model) { earthModel = model; }

	//
	bool load(std::istream &in);
	/*
	bool load(fs::File& f);
	bool load(Stream &s);
	*/
	void reset();

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
