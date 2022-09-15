// LocationDataSource.h
//

#ifndef __LOCATION_DATA_SOURCE_H__
#define __LOCATION_DATA_SOURCE_H__

struct ILocationDataSource
{
    virtual void begin() = 0;
    virtual void end() = 0;

    virtual bool available() = 0;
    virtual int read() = 0;
};


//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource();



#endif // __LOCATION_DATA_SOURCE_H__
