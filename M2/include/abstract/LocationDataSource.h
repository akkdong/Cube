// LocationDataSource.h
//

#ifndef __LOCATION_DATA_SOURCE_H__
#define __LOCATION_DATA_SOURCE_H__

struct ILocationDataSource
{
    virtual void begin(std::function<void (void)> receiveCb = nullptr) = 0;
    virtual void end() = 0;

    virtual int available() = 0;
    virtual int read() = 0;
};


//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource();



#endif // __LOCATION_DATA_SOURCE_H__
