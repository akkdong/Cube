// CriticalSection.h
//

#ifndef __CRITICAL_SECTION_H__
#define __CRITICAL_SECTION_H__

#include "critical_section.h"


/////////////////////////////////////////////////////////////////////////////////
// class CriticalSection

class CriticalSection
{
public:
    CriticalSection();
    virtual ~CriticalSection();

public:
    void enter();
    void leave();

protected:
    critical_section_t  lockHandle;
};


#endif // __CRITICAL_SECTION_H__
