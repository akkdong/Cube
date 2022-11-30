// Display.h
//

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Task.h"
#include "CriticalSection.h"


//////////////////////////////////////////////////////////////////
// class Display

class Display : public CriticalSection/*, public TaskHandler*/
{
public:
    Display();
    virtual ~Display();

public:
    int         begin();

    void        update();

    bool        isDirty() { return dirty; }
    void        setDirty(bool flag) { dirty = flag; }

protected:
    class TimerHandler : public TaskProcHandler
    {
    public:
        TimerHandler(Display* _disp) : disp(_disp) {}

    protected:
        void TaskProc() override;

    protected:
        Display* disp;
    };

    class UpdateHandler : public TaskProcHandler
    {
    public:
        UpdateHandler(Display* _disp) : disp(_disp) {}

    protected:
        void TaskProc() override;

    protected:
        Display* disp;
    };    
    //void        TaskProc() override;
    

protected:
    TaskClass       taskTimerHandler;
    TaskClass       taskUpdateHandler;

    TimerHandler    timerHandler;
    UpdateHandler   updateHandler;

    volatile bool   dirty;
};


#endif // __DISPLAY_H__
