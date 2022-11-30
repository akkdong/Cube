// utils_time.h
//

#ifndef __UTILS_TIME_H__
#define __UTILS_TIME_H__

#include <stddef.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif


inline uint32_t get_tick() { 
    return SDL_GetTicks();
}

inline uint32_t millis() {
    return SDL_GetTicks();
}

inline void delay(size_t ms) {
    SDL_Delay(ms);
}

#ifdef __cplusplus
}
#endif

#endif // __UTILS_TIME_H__
