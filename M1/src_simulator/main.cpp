// main-simulator.cpp
//

#include <stdio.h>
#include <lvgl.h>

#include "device_defines.h"
#include "lv_hal_porting.h"
#include "lv_app.h"

static app_conf_t app_conf;

int main(void)
{
    lv_init();
    lv_hal_setup();

    app_config_init(&app_conf);
    app_init();
    app_loop();

    return 0;
}
