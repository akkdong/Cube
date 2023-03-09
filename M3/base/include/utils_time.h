// utils_time.h
//

#ifndef __UTILS_TIME_H__
#define __UTILS_TIME_H__

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif


inline uint32_t get_tick() { 
    return millis();
}

#ifdef __cplusplus
}
#endif


#endif // __UTILS_TIME_H__
