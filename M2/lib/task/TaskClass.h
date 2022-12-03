// TaskClass.h
//

#ifndef __TASK_CLASS_H__
#define __TASK_CLASS_H__

#include "task_defines.h"


//////////////////////////////////////////////////////////////////
// TaskHandler interface

struct TaskProcHandler
{
    virtual void    TaskProc() = 0;
};


//////////////////////////////////////////////////////////////////
// class TaskClass

class TaskClass
{
public:
    TaskClass();
    virtual ~TaskClass();

public:
    bool                create(TaskProcHandler* procHandler);
    void                destroy();

    void                setName(const char* name) { taskName = name; }
    void                setStackSize(size_t size) { stackSize = size; }
    void                setPriority(task_priority_t priority) { taskPriority = priority; }

public:
    TaskProcHandler*    procHandler;

protected:
    task_handle_t       taskHandle;

    const char*         taskName;
    size_t              stackSize;
    task_priority_t     taskPriority;
};
 


#endif // __TASK_CLASS_H__
