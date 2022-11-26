// DeviceRepository.h
//


#include "app_defines.h"
#include "app.h"



////////////////////////////////////////////////////////////////////////////////
// DeviceRepository

class DeviceRepository
{
protected:
    DeviceRepository();

public:
    static DeviceRepository&    instance();

    bool                        loadPref();
    bool                        savePref();

	void						reset();

	DeviceContext*				getContext() { return contextPtr; }

protected:

protected:
    DeviceContext*              contextPtr;

    static const VarioTone      defaultTone[MAX_VARIO_TONE];
};
