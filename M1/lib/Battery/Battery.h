// Battery.h
//

#ifndef __BATTERY_H__
#define __BATTERY_H__


//////////////////////////////////////////////////////////////////////////////////////////////
//

class Battery
{
public:
    Battery() {
    }

public:
    int update() {
        return 0;
    }

    float getVoltage() {
        return 3.7f;
    }
};

#endif // __BATTERY_H__