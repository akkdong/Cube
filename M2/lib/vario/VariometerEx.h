// VariometerEx.h
//

#include "Variometer.h"
#include "TaskBase.h"
#include "CriticalSection.h"


////////////////////////////////////////////////////////////////////////////////////////
//

class VariometerEx : public TaskBase, public Variometer, public CriticalSection
{
public:
    VariometerEx() : TaskBase("Vario", 4 * 1024, 10) {}

public:
	int				begin(IBarometer* baro, IVarioFilter* filter) override;

	int				update() override;
	void			resetUpdate() override;

protected:
    void            TaskProc() override;

protected:
    volatile int    updateStatus;
};






