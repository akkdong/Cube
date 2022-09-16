// utils.h
//

#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stddef.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////
//

#ifdef ARDUINO
inline uint32_t get_tick() { return millis(); }
#else
inline uint32_t get_tick() { return SDL_GetTicks(); }
#endif



#ifdef __cplusplus
}
#endif

#endif // __UTILS_H__
