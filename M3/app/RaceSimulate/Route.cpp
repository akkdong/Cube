// Route.cpp
//

#include <fstream>

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


bool XcTask::load(std::istream &in)
{
	DynamicJsonDocument doc(2048);
	DeserializationError err = deserializeJson(doc, in);

	if (err)
		return false;

	set(doc);

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
		for (auto it = ar.begin(); it != ar.end(); ++it)
		{
			auto &radius = (*it)["radius"];
			auto &points = (*it)["waypoint"];
			auto &type = (*it)["type"];

			if (!points.isUnbound() && points.is<ArduinoJson::JsonObject>())
			{
				auto& obj = points.as<ArduinoJson::JsonObject>();
				float lon = (float)obj["lon"];
				float lat = (float)obj["lat"];
				float alt = (float)obj["altSmoothed"];
				const char *name = (const char *)obj["name"];
				const char *desc = (const char *)obj["description"];

				float r = 0;
				if (!radius.isUnbound())
					r = (float)radius;

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
					this->addTurnPoint(pointPtr);
			}
		}
	}

	auto& version = doc["version"];
	if (!version.isUnbound())
	{
		if (version.is<int>())
		{
			int v = version.as<int>();
		}
	}

	if (!sss.isUnbound() && sss.is<ArduinoJson::JsonObject>())
	{
		// type
		// direction
		// timeGates
		//
		// this->setGateOpen();
	}

	if (!goal.isUnbound() && goal.is<ArduinoJson::JsonObject>())
	{
		// type
		// deadline
		//
		// this->setDeadLine();
	}

	if (!model.isUnbound())
	{
		const char *str = (const char *)model;
		if (strcmp(str, "WGS84") == 0)
			this->setEarthModel(XcTask::WGS84);
	}
}
