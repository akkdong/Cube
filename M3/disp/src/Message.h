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

    MSG_KEY_PRESSED,
    MSG_KEY_LONG_PRESSED,
    MSG_KEY_RELEASED,

    MSG_TOUCH_PRESSED,
    MSG_TOUCH_LONG_PRESSED,
    MSG_TOUCH_RELEASED,

    MSG_SHUTDOWN,
};

struct Message
{
    uint16_t    code;
    uint16_t    data;
};


#endif // __MESSAGE_H__
