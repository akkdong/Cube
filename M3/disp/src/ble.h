// ble.h
//

#ifndef __M3_BLE_H__
#define __M3_BLE_H__

#include <Arduino.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif 

///////////////////////////////////////////////////////////////////////////////////////
//

bool    ble_isConnected();
int     ble_writeBuffered(uint8_t ch);
void    ble_flush();

size_t  ble_press(uint8_t key);
size_t  ble_release(uint8_t key);


#ifdef __cplusplus
}
#endif

#endif // __M3_BLE_H__
