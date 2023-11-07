// utils_geographiclib.h
//

#pragma once

#include <GeographicLib/Geodesic.hpp>

using GeographicLib::Math;


///////////////////////////////////////////////////////////////////////////////
//

enum class EarthModel {
	FAISPHERE,
	WGS84,
};


///////////////////////////////////////////////////////////////////////////////
//

void geod_setEarthModel(EarthModel model);

Math::real geod_direct(Math::real lat1, Math::real lon1, Math::real azimuth, Math::real dist, Math::real &lat, Math::real &lon);
Math::real geod_inverse(Math::real lat1, Math::real lon1, Math::real lat2, Math::real lon2, Math::real &dist);
