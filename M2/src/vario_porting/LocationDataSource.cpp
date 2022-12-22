// LocationDataSource.cpp
//

#include <Arduino.h>

#include "abstract/LocationDataSource.h"
#include "logger.h"

#include "TaskBase.h"
#include "CriticalSection.h"

#ifndef USE_NMEALOG
#define USE_NMEALOG       (0)
#endif

#if USE_NMEALOG
#include "nmea_log2.h"

#define MAX_BUFFER      (128)
#endif


////////////////////////////////////////////////////////////////////////////////////////////
//

#if USE_NMEALOG
class LocalFileDataSource : public ILocationDataSource, public TaskBase, public CriticalSection
#else
class LocalFileDataSource : public ILocationDataSource
#endif
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

    #if USE_NMEALOG
    void            pump();

    void            TaskProc() override;
    #endif

protected:
    #if USE_NMEALOG
    uint32_t        startTick;

    uint32_t        totalSegment;
    uint32_t        activeSegment;
    uint32_t        segmentLength;
    uint32_t        segmentOffset;
    const char*     segmentPtr;

    char            buffer[MAX_BUFFER];
    volatile size_t front;
    volatile size_t rear;
    #endif

    //
    std::function<void (void)> _onReceive;
};




////////////////////////////////////////////////////////////////////////////////////////////
//

LocalFileDataSource::LocalFileDataSource()
    : _onReceive(nullptr)
#if USE_NMEALOG
    , TaskBase("L", 2 * 1024, 2)
#endif
{
    #if USE_NMEALOG
    totalSegment = sizeof(nmea_log) / sizeof(nmea_log[0]);
    #endif 
}

// send one segment every second.
// segment index is defined as time difference from when begin was called.


void LocalFileDataSource::begin(std::function<void (void)> receiveCb)
{
    _onReceive = receiveCb;

    #if USE_NMEALOG
    activeSegment = (uint32_t)-1;
    segmentLength = 0;
    segmentOffset = 0;
    segmentPtr = nullptr;
    front = rear = 0;

    create();
    #else
    if (_onReceive)
        Serial2.onReceive(_onReceive);
    #endif
}

void LocalFileDataSource::end()
{
}

int LocalFileDataSource::available()
{
    #if !USE_NMEALOG
    return Serial2.available();
    #else
    //pump();
    
    if (front == rear)
        return 0;

    return 1;
    #endif
}

int LocalFileDataSource::read()
{
    #if !USE_NMEALOG
    return Serial2.read();
    #else
    if (!available())
        return -1;

    enter();
    int ch = buffer[rear];
    rear = (rear + 1) % MAX_BUFFER;
    leave();
    //USBSerial.write(ch);

    return ch;
    #endif
}

int LocalFileDataSource::peek()
{
    #if !USE_NMEALOG
    return -1;
    #else
    int ch = -1;
    enter();
    if (front != rear)
        ch = buffer[rear];
    leave();
    return ch;
    #endif
}

#if USE_NMEALOG

void LocalFileDataSource::pump()
{
    uint32_t tick = millis();
    uint32_t segment = ((tick - startTick) / 1000) % totalSegment;

    if (segment != activeSegment)
    {
        activeSegment = segment;
        segmentPtr = nmea_log[segment];
        segmentLength = strlen(segmentPtr);
        segmentOffset = 0;
    }

    uint32_t segmentTick = (tick - startTick) % 1000;
    // 9600ch : 1000ms = x : segmentTick
    uint32_t charCount = 9600 * segmentTick / 1000;

    for (; segmentOffset < charCount && segmentOffset < segmentLength; segmentOffset++)
    {
        size_t index = (front + 1) % MAX_BUFFER;
        if (rear == index)
            rear = (rear + 1) % MAX_BUFFER;

        buffer[front] = segmentPtr[segmentOffset];
        front = index;
    }

    if (front != rear && _onReceive != nullptr)
        _onReceive();
}

void LocalFileDataSource::TaskProc()
{
    startTick = millis();

    while(1)
    {
        enter();
        pump();
        leave();

        vTaskDelay(1);
    }
}

#endif // USE_NMEALOG

//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource()
{
    static LocalFileDataSource ds;

    return (ILocationDataSource *)&ds;
}
