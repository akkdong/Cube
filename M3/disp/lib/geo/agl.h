// agl.h
//

#ifndef __AGLCLASS_H__
#define __AGLCLASS_H__

#include <Arduino.h>

#include "FS.h"
#include "SdCard.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class AGLClass
{
public:
	AGLClass();
	~AGLClass();

public:
	double			getGroundLevel(double lat, double lon);

private:
	int             openFile(const char * name);
	void            closeFile();

	double			getGroundLevel(File * file, double lat, double lon);

private:
	int16_t	        mLat, mLon;
	File	        mFile;
};

#endif // __AGLCLASS_H__
