// LocationDataSource.cpp
//

#include <Arduino.h>
#include <functional>

#include "abstract/LocationDataSource.h"
#include "logger.h"



////////////////////////////////////////////////////////////////////////////////////////////
//

class LocalFileDataSource : public ILocationDataSource
{
public:
    LocalFileDataSource();

public:
    void            begin(std::function<void (void)> receiveCb = nullptr) override;
    void            end() override;

    int             available() override;
    int             read() override;

protected:
    int             peek();

protected:
    std::function<void (void)> _onReceive;
};




////////////////////////////////////////////////////////////////////////////////////////////
//

LocalFileDataSource::LocalFileDataSource()
{
}

void LocalFileDataSource::begin(std::function<void (void)> receiveCb)
{
    if (receiveCb)
        Serial2.onReceive(receiveCb);
}

void LocalFileDataSource::end()
{
}

int LocalFileDataSource::available()
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
