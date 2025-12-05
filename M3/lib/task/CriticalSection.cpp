// CriticalSection.cpp
//

#include "CriticalSection.h"


/////////////////////////////////////////////////////////////////////////////////
// class CriticalSection 

CriticalSection::CriticalSection()
{
    lockHandle = xSemaphoreCreateMutex();
}

CriticalSection::~CriticalSection()
{
    vSemaphoreDelete(lockHandle);
}

void CriticalSection::enter()
{
    xSemaphoreTake(lockHandle, portMAX_DELAY);
}

void CriticalSection::leave()
{
    xSemaphoreGive(lockHandle);
}
