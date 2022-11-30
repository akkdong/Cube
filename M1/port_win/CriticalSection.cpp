// CriticalSection.cpp
//

#include "CriticalSection.h"


/////////////////////////////////////////////////////////////////////////////////
// class CriticalSection 

CriticalSection::CriticalSection()
{
    lockHandle = SDL_CreateMutex();
}

CriticalSection::~CriticalSection()
{
    SDL_DestroyMutex(lockHandle);
}

void CriticalSection::enter()
{
    SDL_mutexP(lockHandle);
}

void CriticalSection::leave()
{
    SDL_mutexV(lockHandle);
}
