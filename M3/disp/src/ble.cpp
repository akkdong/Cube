// ble.cpp
//

#include <Arduino.h>
#include "BLEVario.h"
#include "ble.h"

//
//
//

BLEVario        bleDevice;


//
//
//

bool ble_isConnected()
{
	return bleDevice.isConnected();
}

int  ble_writeBuffered(uint8_t ch)
{
	return bleDevice.writeBuffered(ch);
}

void ble_flush()
{
	bleDevice.flush();
}

size_t ble_press(uint8_t key)
{
	return bleDevice.press(key);
}

size_t ble_release(uint8_t key)
{
	return bleDevice.release(key);
}
