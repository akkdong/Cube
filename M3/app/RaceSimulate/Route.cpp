// Route.cpp
//

#include <fstream>
#include <GeographicLib/Geodesic.hpp>

#include "Route.h"
#undef min
#undef max
#include "bfgs.hpp"



///////////////////////////////////////////////////////////////////////////////
// class XcTurnPoint

XcTurnPoint::XcTurnPoint()
{
}

XcTurnPoint::XcTurnPoint(Type _type, double _r, const char *name, double lat, double lon, double alt, const char *desc)
	: XcPoint(name, lat, lon, alt, desc)
	, type(_type)
	, radius(_r)
	, theta(0)
{

}

XcTurnPoint::XcTurnPoint(Type _type, double _r, XcPoint& point)
	: XcPoint(point)
	, type(_type)
	, radius(_r)
	, theta(0)
{

}


///////////////////////////////////////////////////////////////////////////////
// class XcRoute

XcRoute::XcRoute()
{
	reset();
}

XcTurnPointPtr XcRoute::getTurnPoint(size_t index)
{
	if (index < points.size())
		return points[index];

	return nullptr;
}

void XcRoute::reset()
{
	points.clear();

	totalDist = 0;
	optimizedDist = 0;
}


void XcRoute::optimize(size_t startPoint, double lat_current, double lon_current)
{
	size_t remainPoints = this->points.size() - startPoint;
	auto totalLength = [&, this](const Eigen::VectorXd &theta) -> double {
		//
		Eigen::VectorXd lat_proj = Eigen::VectorXd(remainPoints);
		Eigen::VectorXd lon_proj = Eigen::VectorXd(remainPoints);
		auto &geod = GeographicLib::Geodesic::WGS84();

		for (size_t i = 0; i < (size_t)theta.size(); i++)
		{
			auto& ptr = this->points[i + startPoint];
			double lat, lon, radius;
			double lat_target, lon_target;

			if (i == 0)
			{
				lat = lat_current;
				lon = lon_current;
				radius = 0;
			}
			else
			{
				lat = ptr->getLatitude();
				lon = ptr->getLongitude();
				radius = ptr->getRadius();
			}

			geod.Direct(lat, lon, theta[i], radius, lat_target, lon_target);
			lat_proj[i] = lat_target;
			lon_proj[i] = lon_target;
		}

		//
		double totalDist = 0.0;

		for (int i = 0; i < theta.size() - 1; i++)
		{
			double dist;
			geod.Inverse(lat_proj[i], lon_proj[i], lat_proj[i + 1], lon_proj[i + 1], dist);
			totalDist += dist;
		}

		return totalDist;
	};

	Eigen::VectorXd theta(remainPoints);
	for (size_t i = 0; i < remainPoints; ++i)
		theta[i] = points[i + startPoint]->theta;

	bfgs::optimizer<decltype(totalLength)> o(totalLength, theta);
	bfgs::result result = o.optimize();

	for (size_t i = startPoint; i < points.size(); ++i)
		points[i]->theta = result.x[i - startPoint];

	this->optimizedDist = result.fval;
}

void XcRoute::optimize(size_t startPoint)
{
	if (this->points.size() > 1 && startPoint < this->points.size())
	{
		auto ptr = this->points[startPoint];
		optimize(startPoint, ptr->getLatitude(), ptr->getLongitude());
	}
}

void XcRoute::calcTotalDistance()
{
	auto &geod = GeographicLib::Geodesic::WGS84();
	double lat_last, lon_last;

	this->totalDist = 0;
	for (size_t i = 0; i < this->points.size(); i++)
	{
		auto ptr = this->points[i];
		double lat = ptr->getLatitude();
		double lon = ptr->getLongitude();

		if (i > 0)
		{
			double dist;
			geod.Inverse(lat_last, lon_last, lat, lon, dist);
			this->totalDist += dist;
		}

		lat_last = lat;
		lon_last = lon;
	}
}


///////////////////////////////////////////////////////////////////////////////
// class XcTask

XcTask::XcTask()
{
	reset();
}


bool XcTask::load(std::istream &in)
{
	DynamicJsonDocument doc(2048);
	DeserializationError err = deserializeJson(doc, in);

	if (err)
		return false;

	reset();
	set(doc);

	route.calcTotalDistance();
	route.optimize();

	return true;
}

/*
bool XcTask::load(fs::File &f)
{
	return false;
}

bool XcTask::load(Stream &s)
{
	return false;
}
*/

void XcTask::reset()
{
	version = 1;
	taskType = CLASSIC;
	gateOpen = 0;
	deadLine = 0;
	earthModel = WGS84;

	route.reset();
}


void XcTask::set(JsonDocument &doc)
{
	auto &taskType = doc["taskType"];
	auto &turnPoints = doc["turnpoints"];
	auto &sss = doc["sss"];
	auto &goal = doc["goal"];
	auto &model = doc["earthModel"];

	if (!taskType.isUnbound())
	{
		this->setTaskType(XcTask::COMPETITION);
	}

	if (!turnPoints.isUnbound() && turnPoints.is<ArduinoJson::JsonArray>())
	{
		auto& ar = turnPoints.as< ArduinoJson::JsonArray>();
		size_t size = ar.size(), index = 0;
		route.points.assign(size, nullptr);

		for (auto it = ar.begin(); it != ar.end(); ++it, ++index)
		{
			auto &radius = (*it)["radius"];
			auto &points = (*it)["waypoint"];
			auto &type = (*it)["type"];

			if (!points.isUnbound() && points.is<ArduinoJson::JsonObject>())
			{
				auto& obj = points.as<ArduinoJson::JsonObject>();
				double lon = (double)obj["lon"];
				double lat = (double)obj["lat"];
				double alt = (double)obj["altSmoothed"];
				const char *name = (const char *)obj["name"];
				const char *desc = (const char *)obj["description"];

				double r = 0;
				if (!radius.isUnbound())
					r = (double)radius;

				XcTurnPoint::Type t = XcTurnPoint::TURN;
				if (!type.isUnbound())
				{
					const char *typeStr = (const char *)type;
					if (strcmp(typeStr, "TASKOFF") == 0)
						t = XcTurnPoint::TAKEOFF;
					else if (strcmp(typeStr, "SSS") == 0)
						t = XcTurnPoint::SSS;
					else if (strcmp(typeStr, "ESS") == 0)
						t = XcTurnPoint::ESS;
				}

				XcTurnPointPtr pointPtr = std::make_shared<XcTurnPoint>(type, r, name, lat, lon, alt, desc);
				if (pointPtr)
					route.points[index] = pointPtr;
			}
		}
	}

	auto& version = doc["version"];
	if (!version.isUnbound())
	{
		if (version.is<int>())
			this->version = version.as<int>();
	}

	if (!sss.isUnbound() && sss.is<ArduinoJson::JsonObject>())
	{
		// type
		// direction
		// timeGates
		//
		this->setGateOpen(0);
	}

	if (!goal.isUnbound() && goal.is<ArduinoJson::JsonObject>())
	{
		// type
		// deadline
		//
		this->setDeadLine(0);
	}

	if (!model.isUnbound())
	{
		const char *str = (const char *)model;
		if (strcmp(str, "WGS84") == 0)
			this->setEarthModel(XcTask::WGS84);
	}
}
