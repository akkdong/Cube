// SpeedCalculator.h
//

#include "SpeedCalculator.h"


////////////////////////////////////////////////////////////////////////////////////////
// class SpeedCalculator

SpeedCalculator::SpeedCalculator()
    : speedSum(0.0f)
    , speedAvg(0.0f)
    , speedCount(0)
    , limitCount(40)
{
}


void SpeedCalculator::begin(uint32_t updatePeriod, uint32_t updateRate)
{
    speedSum = speedAvg = 0.0f;
    speedCount = 0;
    limitCount = (uint32_t)((1000.0f / updateRate) * (updatePeriod / 1000.0f));
}

int SpeedCalculator::add(float speed)
{
    speedSum += speed;

    if (++speedCount < limitCount)
        return 0;

    // calculate average & reset
    speedAvg = speedSum / speedCount;
    speedSum = 0.0f;
    speedCount = 0;

    return 1;
}
