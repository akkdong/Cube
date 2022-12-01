// VariometerEx.h
//

#include "Variometer.h"
#include "TaskClass.h"
#include "CriticalSection.h"


////////////////////////////////////////////////////////////////////////////////////////
//

class VariometerEx : public TaskProcHandler, public Variometer, public CriticalSection
{
public:
    VariometerEx() /*: TaskBase("Vario", 4 * 1024, 10)*/ {}

public:
	int				begin(IBarometer* baro, IVarioFilter* filter) override;

	int				update() override;
	void			resetUpdate() override;

protected:
    void            TaskProc() override;

    TaskClass       task;
    volatile int    updateStatus;
};






