// LocationDataSource.cpp
//

#include <SDL2/SDL.h>

#include "abstract/LocationDataSource.h"
#include "logger.h"
#include "nmea_log.h"


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
    const char*     nextPtr;
    unsigned int    lastTick;
    int             lineCount;
};



////////////////////////////////////////////////////////////////////////////////////////////
//

LocalFileDataSource::LocalFileDataSource()
{
    nextPtr = NULL;    
}

void LocalFileDataSource::begin()
{
    LOGi("LocalFileDataSource::begin()");
    nextPtr = nmea_log;
    lastTick = SDL_GetTicks();
    lineCount = 0;
    LOGi("  nmea_log = %c%c%c%c%c%c", nextPtr[0], nextPtr[1], nextPtr[2], nextPtr[3], nextPtr[4], nextPtr[5]);
}

void LocalFileDataSource::end()
{
}

bool LocalFileDataSource::available()
{
    int ch = peek();
    if (ch < 0)
        return false;

    if (lineCount > 1)
    {
        if (SDL_GetTicks() - lastTick < 200)
            return false;

        lineCount = 0;
    }
    
    return true;
}

int LocalFileDataSource::read()
{
    if (!available())
        return -1;

    int ch = peek();
    if (ch == '\n')
    {
        lineCount += 1;
        lastTick = SDL_GetTicks();
    }
    nextPtr += 1;

    return ch;
}

int LocalFileDataSource::peek()
{
    if (!nextPtr)
        return -1;

    if (!*nextPtr)
        nextPtr = nmea_log;

    if (!*nextPtr)
        return -1;

    return *nextPtr;
}


//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource()
{
    static LocalFileDataSource ds;

    return (ILocationDataSource *)&ds;
}
