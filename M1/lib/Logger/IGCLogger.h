// IGCLogger.h
//

#ifndef __IGC_LOGGER_H__
#define __IGC_LOGGER_H__

#include <stdint.h>
#include <time.h>


/////////////////////////////////////////////////////////////////////////////
// class IGCLogger

class IGCLogger
{
public:
    IGCLogger() {}

public:
    bool begin(time_t date) {
        return false;
    }
    void end(time_t date) {        
    }

    bool isLogging() {
        return false;
    }

    void updateBaroAltitude(float altitude) {        
    }

    int write(uint8_t ch) {
        return 1;
    }

};

#endif // __IGC_LOGGER_H__
