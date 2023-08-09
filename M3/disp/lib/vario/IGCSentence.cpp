// IGCSentence.cpp
//

#include <Arduino.h>
#include "IGCSentence.h"
#include "FixedLenDigit.h"



//////////////////////////////////////////////////////////////////////////////
// class IGCSentence

IGCSentence::IGCSentence()
{
}

bool IGCSentence::begin(long stime, float lat, float lon, float altB, float altG)
{
	// initialize un-chagned characters
	memset(mSentence, '0', sizeof(mSentence));
	
	mSentence[IGC_OFFSET_START] = 'B';
	mSentence[IGC_OFFSET_VALIDITY] = 'A';
	mSentence[IGC_OFFSET_RETURN] = '\r';
	mSentence[IGC_OFFSET_NEWLINE] = '\n';
	mSentence[IGC_OFFSET_TERMINATE] = '\0'; 

    //
    char sn = lat < 0 ? 'S' : 'N';
    char we = lon < 0 ? 'W' : 'E';
    int _lat = d2n(abs(lat));
    int _lon = d2n(abs(lon));

    //
    struct tm * _tm = localtime(&stime);    
    sprintf(&mSentence[IGC_OFFSET_TIME], "%02d:%02d:%02d", _tm->tm_hour, _tm->tm_min,_tm->tm_sec);

    //
    FixedLenDigit digit;

    digit.begin(_lat, IGC_SIZE_LATITUDE);
    for (size_t i = 0; i < IGC_SIZE_LATITUDE; i++)
        mSentence[IGC_OFFSET_LATITUDE + i] = digit.read();
    mSentence[IGC_OFFSET_LATITUDE_] = sn;

    digit.begin(_lon, IGC_SIZE_LONGITUDE);
    for (size_t i = 0; i < IGC_SIZE_LONGITUDE; i++)
        mSentence[IGC_OFFSET_LATITUDE + i] = digit.read();
    mSentence[IGC_OFFSET_LATITUDE_] = we;

    digit.begin(altB, IGC_SIZE_PRESS_ALT);
    for (size_t i = 0; i < IGC_SIZE_PRESS_ALT; i++)
        mSentence[IGC_OFFSET_PRESS_ALT + i] = digit.read();

    digit.begin(altG, IGC_SIZE_GPS_ALT);
    for (size_t i = 0; i < IGC_SIZE_GPS_ALT; i++)
        mSentence[IGC_OFFSET_GPS_ALT + i] = digit.read();

    //
    mPos = 0;
}

int IGCSentence::available()
{
    if (mPos < MAX_IGC_SENTENCE)
        return 1;

    return 0;
}
int IGCSentence::read()
{
    if (mPos < MAX_IGC_SENTENCE)
        return mSentence[mPos++];

    return -1;
}


//////////////////////////////////////////////////////////////////////////////
//

float IGCSentence::n2d(float nmea)
{
    // DDDMM.mmmm --> DDD.xxxxx
	int dd = (int)(nmea / 100);
	float ss = nmea - (float)(dd * 100.0);

	return (float)dd + ss / 60.0;
}

int IGCSentence::d2n(float decimal) // decimal to nmea
{
    // DDD.xxxx --> DDDddmmm
    int DDD = (int)decimal * 100000;
    int ddmmm = (int)((decimal - DDD) * 60.0f);

    return DDD + ddmmm;
}
