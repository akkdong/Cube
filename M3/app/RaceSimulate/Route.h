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
	XcBasePoint &getProjection() { return proj; }

	double getDistance(double lat, double lon); // distance from (lat,lon) to center of point
	bool inside(double lat, double lon); // is inside or outside of cylinder

protected:
	Type type;
	double radius;
	double theta;

	XcBasePoint proj;
};

typedef std::shared_ptr<XcTurnPoint> XcTurnPointPtr;


///////////////////////////////////////////////////////////////////////////////
// class XcRoute

class XcRoute
{
public:
    XcRoute();

	enum EarthModel {
		FAISPHERE,
		WGS84,
	};


	XcTurnPointPtr getTurnPoint(size_t index);
	size_t getTurnPointCount() { return points.size(); }
	double getTotalDistance() { return totalDist; }
	double getOptimizedDistance() { return optimizedDist; }
	EarthModel getEatchModel() { return earthModel; }

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

	EarthModel earthModel;
};



///////////////////////////////////////////////////////////////////////////////
// class XcTask

class XcTask : public XcRoute
{
public:
	XcTask();

	enum TaskType {
		CLASSIC,
		COMPETITION,
	};

	//
	TaskType getTaskType() { return taskType; }
	time_t getGateOpen() { return gateOpen; }
	time_t getDeadLine() { return deadLine; }

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
};


#endif // __XC_ROUTE_H__
