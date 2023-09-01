// Message.h
//

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdint.h>


enum MessageCode {
    MSG_NONE,

    MSG_UPDATE_GPS, // MSG_UPDATE_NMEA
    MSG_UPDATE_VARIO,
    MSG_UPDATE_ANNUNCIATOR,
    MSG_UPDATE_TH,
    MSG_UPDATE_BAT,

    MSG_INVALIDATE,

    MSG_GPS_FIXED, // data: fixed(1), unfixed(0)
    MSG_TAKEOFF,
    MSG_LANDING,

    MSG_KEY_PRESSED,        // data: key
    MSG_KEY_LONG_PRESSED,
    MSG_KEY_RELEASED,

    MSG_TOUCH_DOWN,      // data: x << 16 | y
    MSG_TOUCH_MOVE,
    MSG_TOUCH_UP,

    // system message
    MSG_SHOW_TOPMENU,
    MSG_SHOW_STATISTIC,
    MSG_SHOW_WIFI,
    MSG_SHOW_ROUTE,
    MSG_SHOW_SETTINGS,
    MSG_FALLBACK,

    MSG_PRE_SHUTDOWN,
    MSG_SHUTDOWN,
};

struct Message
{
    uint32_t    code;
    uint32_t    data;
};


#endif // __MESSAGE_H__
