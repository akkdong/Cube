// LocationDataSource.cpp
//

#include <SDL2/SDL.h>
#include <functional>

#include "abstract/LocationDataSource.h"
#include "logger.h"
#include "nmea_log.h"
#include "nmea_log2.h"
#include "utils.h"

#include "Task.h"
#include "CriticalSection.h"

#define MAX_BUFFER      (128)


////////////////////////////////////////////////////////////////////////////////////////////
//

class LocalFileDataSource : public ILocationDataSource, public TaskProcHandler, public CriticalSection
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
    void            pump();

    void            TaskProc() override;

protected:
    //const char*     nextPtr;
    //unsigned int    lastTick;
    //int             lineCount;

    uint32_t        startTick;

    uint32_t        totalSegment;
    uint32_t        activeSegment;
    uint32_t        segmentLength;
    uint32_t        segmentOffset;
    const char*     segmentPtr;

    char            buffer[MAX_BUFFER];
    volatile size_t front;
    volatile size_t rear;

    TaskClass       task;

    //
    std::function<void (void)> _onReceive;
};



////////////////////////////////////////////////////////////////////////////////////////////
//

LocalFileDataSource::LocalFileDataSource()
    : _onReceive(nullptr)
{
    //nextPtr = NULL;

    totalSegment = sizeof(nmea_log_segment) / sizeof(nmea_log_segment[0]);
}

void LocalFileDataSource::begin(std::function<void (void)> receiveCb)
{
    LOGi("LocalFileDataSource::begin()");

    //
    _onReceive = receiveCb;

    //nextPtr = nmea_log;
    //lastTick = SDL_GetTicks();
    //lineCount = 0;
    //LOGi("  nmea_log = %c%c%c%c%c%c", nextPtr[0], nextPtr[1], nextPtr[2], nextPtr[3], nextPtr[4], nextPtr[5]);

    activeSegment = (uint32_t)-1;
    segmentLength = 0;
    segmentOffset = 0;
    segmentPtr = nullptr;
    front = rear = 0;

    task.setName("LOC");
    task.setStackSize(2 * 1024);
    task.setPriority(1);
    task.create(this);      
}

void LocalFileDataSource::end()
{
}

int LocalFileDataSource::available()
{
    #if 0
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
    #else
    if (front == rear)
        return 0;

    return 1;
    #endif
}

int LocalFileDataSource::read()
{
    #if 0
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
    #else
    if (!available())
        return -1;

    enter();
    int ch = buffer[rear];
    rear = (rear + 1) % MAX_BUFFER;
    leave();

    return ch;    
    #endif
}

int LocalFileDataSource::peek()
{
    #if 0
    if (!nextPtr)
        return -1;

    if (!*nextPtr)
        nextPtr = nmea_log;

    if (!*nextPtr)
        return -1;

    return *nextPtr;
    #else
    int ch = -1;
    enter();
    if (front != rear)
        ch = buffer[rear];
    leave();
    return ch;
    #endif
}

void LocalFileDataSource::pump()
{
    uint32_t tick = millis();
    uint32_t segment = ((tick - startTick) / 1000) % totalSegment;

    if (segment != activeSegment)
    {
        activeSegment = segment;
        segmentPtr = nmea_log_segment[segment];
        segmentLength = strlen(segmentPtr);
        segmentOffset = 0;

        //printf("\n=================\n");
    }

    uint32_t segmentTick = (tick - startTick) % 1000;
    // 9600 bps => 9600 / 11bit ~= 873 bytes
    // 873 bytes : 1000ms = x : segmentTick
    uint32_t charCount = 873 * segmentTick / 1000;

    for (; segmentOffset < charCount && segmentOffset < segmentLength; segmentOffset++)
    {
        size_t index = (front + 1) % MAX_BUFFER;
        if (rear == index)
        {
            //LOGw("\r\n*F*");
            rear = (rear + 1) % MAX_BUFFER;
        }

        buffer[front] = segmentPtr[segmentOffset];
        //fputc(buffer[front], stdout);
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

        SDL_Delay(1);
    }
}


//////////////////////////////////////////////////////////////////////////////
//

ILocationDataSource* CreateLocationDataSource()
{
    static LocalFileDataSource ds;

    return (ILocationDataSource *)&ds;
}
