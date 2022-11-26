// SpeedCalculator.h
//

#ifndef __SPEED_CALCULATOR_H__
#define __SPEED_CALCULATOR_H__

#include <stdint.h>


////////////////////////////////////////////////////////////////////////////////////////
// class SpeedCalculator

class SpeedCalculator
{
public:
    SpeedCalculator();

public:
    void            begin(uint32_t updatePeriod = 1000, uint32_t updateRate = 25);

    int             add(float speed);
    float           get() { return speedAvg; }

protected:
    float           speedSum;
    float           speedAvg;

    uint32_t        speedCount;
    uint32_t        limitCount;
};


#endif // __SPEED_CALCULATOR_H__
