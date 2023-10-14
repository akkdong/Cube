// WayPoints.h
//

#ifndef __WAY_POINTS_H__
#define __WAY_POINTS_H__

#include <string>
#include <vector>
#include <memory>

typedef std::string String;


///////////////////////////////////////////////////////////////////////////////
// class XcPoint

class XcPoint
{
public:
    XcPoint();
	XcPoint(const char *name, double lat, double lon, double alt, const char *desc = nullptr);

	const char *getName() { return this->name.c_str(); }
	const char *getDescription() { return this->description.c_str(); }
	double getLatitude() { return this->lat; }
	double getLongitude() { return this->lon; }
	double getAltitude() { return this->altitude; }

	void setName(const char *name);
	void setDescription(const char *desc);
	void setLatitude(double lat);
	void setLongitude(double lon);
	void setAltitude(double alt);

protected:
    String name;
    String description;
    double lat;
    double lon;
    double altitude;
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
