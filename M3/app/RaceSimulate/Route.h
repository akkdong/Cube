// Route.h
//

#ifndef __XC_ROUTE_H__
#define __XC_ROUTE_H__

#include <iostream>

#include "utils_geographiclib.h"
#include "WayPoints.h"
#include "ArduinoJson.h"

#define CALC_REPEATCOUNT		(1)

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
	XcTurnPoint(Type type, Math::real radius, const char *name, Math::real lat, Math::real lon, Math::real alt, const char *desc = nullptr);
	XcTurnPoint(Type type, Math::real radius, XcPoint& point);

	Type getType() { return type; }
	Math::real getRadius() { return radius; }
	Math::real getTheta() { return theta; }
	XcBasePoint &getProjection() { return proj; }

	Math::real getDistance(Math::real lat, Math::real lon); // distance from (lat,lon) to center of point
	bool inside(Math::real lat, Math::real lon); // is inside or outside of cylinder

protected:
	Type type;
	Math::real radius;
	Math::real theta;

	XcBasePoint proj;
};

typedef std::shared_ptr<XcTurnPoint> XcTurnPointPtr;


///////////////////////////////////////////////////////////////////////////////
// class XcRoute

class XcRoute
{
public:
    XcRoute();

	//
	XcTurnPointPtr getTurnPoint(size_t index);
	size_t getTurnPointCount() { return points.size(); }
	Math::real getTotalDistance() { return totalDist; }
	Math::real getOptimizedDistance() { return optimizedDist; }

	void addTurnPoint(XcTurnPointPtr pointPtr) { points.push_back(pointPtr); }
	void reset();

	//
	void optimize(size_t startPoint, Math::real lat, Math::real lon);
	void optimize(size_t startPoint = 0);

	void calcTotalDistance();

public:
#if CALC_REPEATCOUNT
	int repeatCount;
#endif

protected:
    std::vector<XcTurnPointPtr> points;

	// 
	Math::real totalDist;
	Math::real optimizedDist;
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

	enum TaskState {
		READY,
		TAKEOFF,
		STARTED,
		FINAL,
		GOAL,
	};

	//
	TaskType getTaskType() { return taskType; }
	time_t getGateOpen() { return gateOpen; }
	time_t getDeadLine() { return deadLine; }

	void setTaskType(TaskType type) { taskType = type; }
	void setGateOpen(time_t t) { gateOpen = t; }
	void setDeadLine(time_t t) { deadLine = t; }

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
	TaskState taskState;
	time_t gateOpen;
	time_t deadLine;
};



#endif // __XC_ROUTE_H__
