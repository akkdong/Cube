// timezone.h
//

#ifndef __TIME_ZONE_H__
#define __TIME_ZONE_H__

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////
//

void setTimeZone(long offset, int daylight);
void setDeviceTime(time_t time);



#ifdef __cplusplus
}
#endif

#endif // __TIME_ZONE_H__
