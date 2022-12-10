// Accumulator.h
//

#include "Accumulator.h"


////////////////////////////////////////////////////////////////////////////////////////
// class Accumulator

Accumulator::Accumulator()
    : sum(0.0f)
    , average(0.0f)
    , accumulateCount(0)
    , limitCount(40)
{
}


void Accumulator::begin(uint32_t updatePeriod, uint32_t updateRate)
{
    sum = average = 0.0f;
    accumulateCount = 0;

    limitCount = (uint32_t)(updatePeriod * updateRate / 1000.0f);
}

int Accumulator::add(float value)
{
    sum += value;

    if (++accumulateCount < limitCount)
        return 0;

    // calculate average & reset
    average = sum / accumulateCount;
    sum = 0.0f;
    accumulateCount = 0;

    return 1;
}
