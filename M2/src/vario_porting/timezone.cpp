// timezone.cpp
//

#include <Arduino.h>
#include <time.h>

#include "timezone.h"
#include "logger.h"


void setTimeZone(long offset, int daylight)
{
    char cst[17] = {0};
    char cdt[17] = "DST";
    char tz[33] = {0};

    if(offset % 3600) {
        sprintf(cst, "UTC%ld:%02u:%02u", offset / 3600, abs((offset % 3600) / 60), abs(offset % 60));
    } else {
        sprintf(cst, "UTC%ld", offset / 3600);
    }

    if(daylight != 3600) {
        long tz_dst = offset - daylight;
        if(tz_dst % 3600){
            sprintf(cdt, "DST%ld:%02u:%02u", tz_dst / 3600, abs((tz_dst % 3600) / 60), abs(tz_dst % 60));
        } else {
            sprintf(cdt, "DST%ld", tz_dst / 3600);
        }
    }

    sprintf(tz, "%s%s", cst, cdt);
    setenv("TZ", tz, 1);
    tzset();
}

void setDeviceTime(time_t time)
{
    struct timeval now = { time, 0 };
    settimeofday(&now, NULL);

    struct tm * _tm = localtime(&time);
    LOGv("set device-time: %d:%d:%d", _tm->tm_hour, _tm->tm_min,_tm->tm_sec);
}
