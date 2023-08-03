// BluetoothManager.h
//

#ifndef __BLUETOOTH_MANAGER_H__
#define __BLUETOOTH_MANAGER_H__

#include <stdint.h>
#include <time.h>


/////////////////////////////////////////////////////////////////////////////////
// class BluetoothManager

class NmeaParser;
class VarioSentence;

class BluetoothManager
{
public:
    BluetoothManager();

public:
    // mode: HID, SPP or HID&SPP
    int                 begin(uint8_t mode, const char* deviceName = nullptr);
    void                end();

    void                update(VarioSentence& varioNmea, NmeaParser& locParser);

    int                 startLogging(time_t date);
    void                stopLogging();

    bool                isConnected();
    bool                available();

    int                 read();
    int                 write(uint8_t ch);
    void                flush();

    void                keyPress(uint8_t key);
    void                keyRelease(uint8_t key);

protected:
    int                 lock_state;
};

#endif // __BLUETOOTH_MANAGER_H__
