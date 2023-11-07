// utils_geographiclib.cpp
//

#include "utils_geographiclib.h"


///////////////////////////////////////////////////////////////////////////////
//

GeographicLib::Geodesic _geod = GeographicLib::Geodesic::WGS84();
EarthModel earthModel = EarthModel::WGS84;



///////////////////////////////////////////////////////////////////////////////
//

void geod_setEarthModel(EarthModel model)
{
	switch (model)
	{
	case EarthModel::FAISPHERE:
		if (earthModel != EarthModel::FAISPHERE)
			_geod = GeographicLib::Geodesic(GeographicLib::Constants::WGS84_a(), 0);
		break;
	case EarthModel::WGS84:
		if (earthModel != EarthModel::WGS84)
			_geod = GeographicLib::Geodesic::WGS84();
		break;
	}
}


Math::real geod_direct(Math::real lat1, Math::real lon1, Math::real azimuth, Math::real dist, Math::real &lat, Math::real &lon)
{
	return _geod.Direct(lat1, lon1, azimuth, dist, lat, lon);
}

Math::real geod_inverse(Math::real lat1, Math::real lon1, Math::real lat2, Math::real lon2, Math::real &dist)
{
	return _geod.Inverse(lat1, lon1, lat2, lon2, dist);
}
