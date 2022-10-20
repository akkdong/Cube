// lv_port_disp.h
//

#ifndef __LV_PORT_DISP_H__
#define __LV_PORT_DISP_H__

#include "board.h"
#include "lvgl.h"
#include "rm68120.h"


#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////////////////////////////////////////
//

void lv_port_disp_init(void);


/////////////////////////////////////////////////////////////////////////////////////////
//

esp_err_t lv_port_tick_init(void);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // __LV_PORT_DISP_H__
