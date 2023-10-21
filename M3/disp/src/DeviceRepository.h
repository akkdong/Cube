// DeviceRepository.h
//

#ifndef __DEVICE_REPOSITORY_H__
#define __DEVICE_REPOSITORY_H__

#include <ArduinoJson.h>
#include "config.h"
#include "DeviceContext.h"


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

    //
	size_t                      updateVSpeedHistory(float speed);
	size_t                      updateTrackHistory();

    size_t                      getVSpeedCount();
    size_t                      getTrackCount();

    void                        resetFlightState();
    void                        resetFlightStats();

protected:
    void                        set(JsonDocument& doc);
    void                        dump();

protected:
    DeviceContext*              contextPtr;

    static const VarioTone      defaultTone[MAX_VARIO_TONE];
};

#endif // __DEVICE_REPOSITORY_H__
