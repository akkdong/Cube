// VariometerEx.h
//

#include "Variometer.h"
#include "TaskBase.h"
#include "CriticalSection.h"
#include "Accumulator.h"


////////////////////////////////////////////////////////////////////////////////////////
//

class Beeper;

class VariometerEx : public Variometer, public TaskBase, public CriticalSection
{
public:
    VariometerEx(Beeper& beeper);

public:
	int				begin(IBarometer* baro, IVarioFilter* filter) override;

	int				update() override;
	void			resetUpdate() override;

    float           getLazyPressure() { return pressureLazy; }
    float           getLazyVario() { return varioLazy; }

protected:
    void            TaskProc() override;

protected:
    Beeper&         beeper;
    volatile int    updateStatus;

    float           pressureLazy;
    float           varioLazy;

    Accumulator     accumulateVario;
    Accumulator     accumulatePressure;
};






