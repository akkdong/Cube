// TwoWireEx.cpp
//

#include "TwoWireEx.h"


//////////////////////////////////////////////////////////////////////////
//

TwoWireEx::TwoWireEx(uint8_t bus_num)
    : TwoWire(bus_num)
{
    lockHandle = xSemaphoreCreateMutex();
}

TwoWireEx::~TwoWireEx()
{
    vSemaphoreDelete(lockHandle);
}

void TwoWireEx::lock()
{
    xSemaphoreTake(lockHandle, portMAX_DELAY);
}

void TwoWireEx::unlock()
{
    xSemaphoreGive(lockHandle);
}


//////////////////////////////////////////////////////////////////////////
//

TwoWireEx    WireEx(0);
