// TwoWireEx.h
//

#ifndef __TWO_WIRE_EX_H__
#define __TWO_WIRE_EX_H__

#include <Arduino.h>
#include <Wire.h>


//////////////////////////////////////////////////////////////////////////
//

class TwoWireEx : public TwoWire
{
public:
    TwoWireEx(uint8_t bus_num);
    ~TwoWireEx();

public:
    void                lock();
    void                unlock();

protected:
    SemaphoreHandle_t   lockHandle;
};


//////////////////////////////////////////////////////////////////////////
//

extern TwoWireEx    WireEx;

#endif // __TWO_WIRE_EX_H__
