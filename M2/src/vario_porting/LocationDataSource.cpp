// LocationDataSource.cpp
//

#include <Arduino.h>

#include "abstract/LocationDataSource.h"
#include "logger.h"

#ifndef USE_NMEALOG
#define USE_NMEALOG       (0)
#endif

#if USE_NMEALOG
#include "nmea_log.h"
#endif


////////////////////////////////////////////////////////////////////////////////////////////
//

class LocalFileDataSource : public ILocationDataSource
{
public:
    LocalFileDataSource();

public:
    void            begin();
    void            end();

    bool            available();
    int             read();

protected:
    int             peek();

protected:
    #if USE_NMEALOG
    const char*     nextPtr;
    unsigned int    lastTick;
    int             lineCount;
    #endif
};




////////////////////////////////////////////////////////////////////////////////////////////
//

LocalFileDataSource::LocalFileDataSource()
{
}

void LocalFileDataSource::begin()
{
    #if USE_NMEALOG
    nextPtr = nmea_log;
    lastTick = millis();
    lineCount = 0;
    #endif
}

void LocalFileDataSource::end()
{
}

bool LocalFileDataSource::available()
{
    #if !USE_NMEALOG
    return Serial2.available();
    #else
    int ch = peek();
    if (ch < 0)
        return false;

    if (lineCount > 1)
    {
        if (millis() - lastTick < 200)
            return false;

        lineCount = 0;
    }
    
    return true;
    #endif
}

int LocalFileDataSource::read()
{
    #if !USE_NMEALOG
    return Serial2.read();
    #else
    if (!available())
        return -1;

    int ch = peek();
    if (ch == '\n')
    {
        lineCount += 1;
        lastTick = millis();
    }
    nextPtr += 1;

    return ch;
    #endif
}

int LocalFileDataSource::peek()
{
    #if !USE_NMEALOG
    return -1;
    #else
    if (!nextPtr)
        return -1;

    if (!*nextPtr)
        nextPtr = nmea_log;

    if (!*nextPtr)
        return -1;

    return *nextPtr;
    #endif
}



//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource()
{
    static LocalFileDataSource ds;

    return (ILocationDataSource *)&ds;
}
