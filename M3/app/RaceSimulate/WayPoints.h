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
	XcPoint(const char *name, float lat, float lon, float alt, const char *desc = nullptr);

	const char *getName() { return this->name.c_str(); }
	const char *getDescription() { return this->description.c_str(); }
	float getLatitude() { return this->lat; }
	float getLongitude() { return this->lon; }
	float getAltitude() { return this->altitude; }

	void setName(const char *name);
	void setDescription(const char *desc);
	void setLatitude(float lat);
	void setLongitude(float lon);
	void setAltitude(float alt);

protected:
    String name;
    String description;
    float lat;
    float lon;
    float altitude;
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
