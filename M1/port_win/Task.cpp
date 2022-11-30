// Task.cpp
//

#include "Task.h"

#include "logger.h"
#include "utils.h"


//////////////////////////////////////////////////////////////////
//

static int _TaskProc(void* param)
{
    TaskClass* pThis = (TaskClass *)param;
    if (pThis->procHandler)
        pThis->procHandler->TaskProc();
    pThis->destroy();

    return 0;
}


//////////////////////////////////////////////////////////////////
// class TaskClass


TaskClass::TaskClass() 
    : procHandler(nullptr)
    , taskHandle(nullptr)
    , taskName("")
    , stackSize(2 * 1024)
    , taskPriority(1)
{
}

TaskClass::~TaskClass()
{
}

bool TaskClass::create(TaskProcHandler* handler)
{
    if (taskHandle != nullptr)
        return false;

    procHandler = handler;
    taskHandle = SDL_CreateThread(_TaskProc, this->taskName, this);

    return taskHandle != nullptr ? true : false;
}

void TaskClass::destroy()
{
    if (taskHandle != nullptr)
        taskHandle = nullptr;
    procHandler = nullptr;
}
