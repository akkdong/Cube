// LocationDataSource.cpp
//

#include <Arduino.h>

#include "abstract/LocationDataSource.h"
#include "logger.h"



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
};




////////////////////////////////////////////////////////////////////////////////////////////
//

LocalFileDataSource::LocalFileDataSource()
{
}

void LocalFileDataSource::begin()
{
}

void LocalFileDataSource::end()
{
}

bool LocalFileDataSource::available()
{
    return Serial2.available();
}

int LocalFileDataSource::read()
{
    return Serial2.read();
}




//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource()
{
    static LocalFileDataSource ds;

    return (ILocationDataSource *)&ds;
}
