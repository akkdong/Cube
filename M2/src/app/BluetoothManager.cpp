// BluetoothManager.cpp
//

#include "board.h"
#include "bsp.h"

#include "BluetoothManager.h"
#include "VarioSentence.h"
#include "LocationParser.h"

#ifdef ARDUINO
#include "BLEVario.h"
extern BLEVario bleDevice;
#endif

/////////////////////////////////////////////////////////////////////////////////
// class BluetoothManager

BluetoothManager::BluetoothManager()
    : lock_state(0)
{

}

int BluetoothManager::begin(uint8_t mode, const char* deviceName)
{
    #ifdef ARDUINO
    bleDevice.setName(deviceName);
    //bleDevice.setMode(mode);
    bleDevice.begin();
    #endif

    return 0;
}

void BluetoothManager::end()
{
}

void BluetoothManager::update(VarioSentence& varioNmea, LocationParser& locParser)
{
    // vario-sentense available?
    if ((lock_state == 0 && varioNmea.available()) || (lock_state == 1))
    {
        lock_state = 1;

        while (varioNmea.available())
        {
            int ch = varioNmea.read();
            if (ch < 0)
                break;

            ble_writeBuffered(ch);
            //Serial.write(ch);

            if (ch == '\n')
            {
                lock_state = 0;
                break;
            }
        }

        if (lock_state == 0 /*|| bleDevice.uartBufferIsFull()*/)
            ble_flush();
    }    

    // nmea-sentense avaialble?
    if ((lock_state == 0 && locParser.availableNmea()) || (lock_state == 2))
    {
        lock_state = 2;

        while (locParser.availableNmea())
        {
            int ch = locParser.readNmea();
            if (ch < 0)
                break;

            ble_writeBuffered(ch);
            //Serial.write(ch);

            if (ch == '\n')
            {
                lock_state = 0;
                break;
            }
        }

        if (lock_state == 0 /*|| bleDevice.uartBufferIsFull()*/)
            ble_flush();
    } 
}

int BluetoothManager::startLogging(time_t date)
{
    return 0;
}

void BluetoothManager::stopLogging()
{
}

bool BluetoothManager::isConnected()
{
    return ble_isConnected();
}

bool BluetoothManager::available()
{
    return false;
}

int BluetoothManager::read()
{
    return -1;
}

int BluetoothManager::write(uint8_t ch)
{
    return ble_writeBuffered(ch);
}

void BluetoothManager::flush()
{
    ble_flush();
}

void BluetoothManager::keyPress(uint8_t key)
{
    ble_press(key);
}

void BluetoothManager::keyRelease(uint8_t key)
{
    ble_release(key);
}
