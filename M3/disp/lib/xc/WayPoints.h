// WayPoints.h
//

#ifndef __WAY_POINTS_H__
#define __WAY_POINTS_H__

#include <string>
#include <vector>
#include <memory>

#include "utils_geographiclib.h"


#ifndef ARDUINO
typedef std::string String;
#endif

class XcWayPoints;
class XcRoute;


///////////////////////////////////////////////////////////////////////////////
// class XcBasePoint

class XcBasePoint
{
	friend class XcWayPoints;
	friend class XcRoute;

public:
	XcBasePoint();
	XcBasePoint(Math::real lat, Math::real lon);

	Math::real getLatitude() { return this->lat; }
	Math::real getLongitude() { return this->lon; }

	void setLatitude(Math::real lat) { this->lat = lat; }
	void setLongitude(Math::real lon) { this->lon = lon; }

protected:
    Math::real lat;
    Math::real lon;
};



///////////////////////////////////////////////////////////////////////////////
// class XcPoint

class XcPoint : public XcBasePoint
{
	friend class XcWayPoints;
	friend class XcRoute;

public:
	XcPoint();
	XcPoint(const char *name, Math::real lat, Math::real lon, Math::real alt, const char *desc = nullptr);

	const char *getName() { return this->name.c_str(); }
	const char *getDescription() { return this->description.c_str(); }
	Math::real getAltitude() { return this->altitude; }

	void setName(const char *name);
	void setDescription(const char *desc);
	void setAltitude(Math::real alt);

protected:
	String name;
	String description;
	Math::real altitude;
};



///////////////////////////////////////////////////////////////////////////////
// class XcWayPoints

class XcWayPoints
{
public:
    XcWayPoints();

    enum UNIT {
        FEET,
        METER
    };

protected:
    int version;
	UNIT unit;
	std::vector<XcPoint> points;
};


#endif // __WAY_POINTS_H__
