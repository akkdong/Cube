// Route.cpp
//

#include <fstream>

#include "Route.h"
#undef min
#undef max
#include "bfgs.hpp"



///////////////////////////////////////////////////////////////////////////////
// class XcTurnPoint

XcTurnPoint::XcTurnPoint()
{
}

XcTurnPoint::XcTurnPoint(Type _type, Math::real _r, const char *name, Math::real lat, Math::real lon, Math::real alt, const char *desc)
	: XcPoint(name, lat, lon, alt, desc)
	, type(_type)
	, radius(_r)
	, theta(0)
{
}

XcTurnPoint::XcTurnPoint(Type _type, Math::real _r, XcPoint& point)
	: XcPoint(point)
	, type(_type)
	, radius(_r)
	, theta(0)
{
}

Math::real XcTurnPoint::getDistance(Math::real lat, Math::real lon)
{
	Math::real dist;
	geod_inverse(this->lat, this->lon, lat, lon, dist);

	return dist;
}

bool XcTurnPoint::inside(Math::real lat, Math::real lon)
{
	Math::real dist;
	geod_inverse(this->lat, this->lon, lat, lon, dist);

	return dist < this->radius ? true : false;
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

#if 0
void XcRoute::optimize(size_t startPoint, Math::real lat_current, Math::real lon_current)
{
	size_t remainPoints = this->points.size() - startPoint;
	auto totalLength = [&, this](const Eigen::VectorXd &theta) -> Math::real {
		//
		Eigen::VectorXd lat_proj = Eigen::VectorXd(remainPoints);
		Eigen::VectorXd lon_proj = Eigen::VectorXd(remainPoints);
		auto &geod = GeographicLib::Geodesic::WGS84();

		for (size_t i = 0; i < (size_t)theta.size(); i++)
		{
			auto& ptr = this->points[i + startPoint];
			Math::real lat, lon, radius;
			Math::real lat_target, lon_target;

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
		Math::real totalDist = 0.0;

		for (int i = 0; i < theta.size() - 1; i++)
		{
			Math::real dist;
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
#else
void XcRoute::optimize(size_t startPoint, Math::real lat_current, Math::real lon_current)
{
	size_t remainPoints = this->points.size() - startPoint;
#if CALC_REPEATCOUNT
	repeatCount = 0;
#endif

	auto totalLength = [&, this](const Eigen::VectorXd &theta) -> Math::real {
		//
		for (size_t i = 0; i < (size_t)theta.size(); i++)
		{
			auto ptr = this->points[i + startPoint];
			Math::real lat, lon, radius;

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

			geod_direct(lat, lon, theta[i], radius, ptr->proj.lat, ptr->proj.lon);
		}

		//
		Math::real totalDist = 0.0;

		for (int i = 0; i < theta.size() - 1; i++)
		{
			auto ptr1 = this->points[i + startPoint];
			auto ptr2 = this->points[i + startPoint + 1];
			Math::real dist;

			geod_inverse(ptr1->proj.lat, ptr1->proj.lon, ptr2->proj.lat, ptr2->proj.lon, dist);
			totalDist += dist;
		}

		//
#if CALC_REPEATCOUNT
		++repeatCount;
#endif

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
#endif

void XcRoute::optimize(size_t startPoint)
{
	if (this->points.size() > 1 && startPoint + 1 < this->points.size())
	{
		auto ptr = this->points[startPoint];
		optimize(startPoint, ptr->getLatitude(), ptr->getLongitude());
	}
}

void XcRoute::calcTotalDistance()
{
	Math::real lat_last, lon_last;

	this->totalDist = 0;
	for (size_t i = 0; i < this->points.size(); i++)
	{
		auto ptr = this->points[i];
		Math::real lat = ptr->getLatitude();
		Math::real lon = ptr->getLongitude();

		if (i > 0)
		{
			Math::real dist;
			geod_inverse(lat_last, lon_last, lat, lon, dist);

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

	calcTotalDistance();
	optimize();

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
	XcRoute::reset();

	version = 1;
	taskType = CLASSIC;
	gateOpen = 0;
	deadLine = 0;
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
		this->points.assign(size, nullptr);

		for (auto it = ar.begin(); it != ar.end(); ++it, ++index)
		{
			auto &radius = (*it)["radius"];
			auto &points = (*it)["waypoint"];
			auto &type = (*it)["type"];

			if (!points.isUnbound() && points.is<ArduinoJson::JsonObject>())
			{
				auto& obj = points.as<ArduinoJson::JsonObject>();
				Math::real lon = (Math::real)obj["lon"];
				Math::real lat = (Math::real)obj["lat"];
				Math::real alt = (Math::real)obj["altSmoothed"];
				const char *name = (const char *)obj["name"];
				const char *desc = (const char *)obj["description"];

				Math::real r = 0;
				if (!radius.isUnbound())
					r = (Math::real)radius;

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
					this->points[index] = pointPtr;
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
		{
			geod_setEarthModel(EarthModel::WGS84);
		}
		else if (strcmp(str, "FAI") == 0)
		{
			geod_setEarthModel(EarthModel::FAISPHERE);
		}
	}
}
