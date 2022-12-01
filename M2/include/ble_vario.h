// ble_vario.h
//

#ifndef __BLE_VARIO_EXTERN_H__
#define __BLE_VARIO_EXTERN_H__

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

#endif // #define __BLE_VARIO_EXTERN_H__

