// TaskBase.cpp
//

#include "TaskBase.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////
// class TaskBase

TaskBase::TaskBase(const char * name, uint32_t depth, UBaseType_t priority) : taskName(name), stackDepth(depth), taskPriority(priority), taskHandle(NULL)
{
}

bool TaskBase::create()
{
	if (taskHandle != NULL)
		return false;

	xTaskCreate(
		_TaskProc,  
		taskName,     // A name just for humans
		stackDepth,   // This stack size can be checked & adjusted by reading the Stack Highwater
		this,  // Parameter
		taskPriority,    // Priority, with (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		&taskHandle);

	return true;
}

bool TaskBase::createPinnedToCore(const BaseType_t xCoreID)
{
	if (taskHandle != NULL)
		return false;

	xTaskCreatePinnedToCore(
		_TaskProc,  
		taskName,     // A name just for humans
		stackDepth,   // This stack size can be checked & adjusted by reading the Stack Highwater
		this,  // Parameter
		taskPriority,    // Priority, with (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		&taskHandle ,  
		xCoreID);

	return true;
}

void TaskBase::destroy()
{
	if (taskHandle)
	{
		vTaskDelete(taskHandle);
		taskHandle = NULL;
	}
}

void TaskBase::_TaskProc(void * param)
{
	((TaskBase *)param)->TaskProc();	
	((TaskBase *)param)->taskHandle = NULL;

	vTaskDelete(NULL);
}
