// Accumulator.h
//

#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__

#include <stdint.h>


////////////////////////////////////////////////////////////////////////////////////////
// class Accumulator

class Accumulator
{
public:
    Accumulator();

public:
    void            begin(uint32_t updatePeriod = 1000, uint32_t updateRate = 25);

    int             add(float value);
    float           get() { return average; }

protected:
    float           sum;
    float           average;

    uint32_t        accumulateCount;
    uint32_t        limitCount;
};


#endif // __ACCUMULATOR_H__
