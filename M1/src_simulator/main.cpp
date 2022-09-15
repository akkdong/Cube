// main-simulator.cpp
//
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <lvgl.h>

#include "device_defines.h"
#include "logger.h"
#include "lv_disp.h"
#include "lv_app.h"

static app_conf_t app_conf;


/////////////////////////////////////////////////////////////////////////////
//


////////////////////////////////////////////////////////////////////////////
//

int main(void)
{
    srand(time(NULL));

    lv_init();
    lv_hal_setup();

    app_config_init(&app_conf);
    app_init();

    lv_hal_loop();

    return 0;
}
#endif
