// Task.cpp
//

#include "Task.h"



//////////////////////////////////////////////////////////////////
//

static void _TaskProc(void* param)
{
    TaskClass* pThis = (TaskClass *)param;
    if (pThis->procHandler)
        pThis->procHandler->TaskProc();
    pThis->destroy();
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

    xTaskCreate(_TaskProc,
        taskName,
        stackSize,
        this,
        taskPriority, // Priority, with (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        &taskHandle) ;
}

void TaskClass::destroy()
{
    if (taskHandle != nullptr)
        vTaskDelete(taskHandle);

    taskHandle = nullptr;
    procHandler = nullptr;
}