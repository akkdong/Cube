// main-simulator.cpp
//

#include <stdio.h>
#include <lvgl.h>

#include "lv_hal_porting.h"
#include "lv_app.h"

int main(void)
{
    lv_init();

    app_setup();
    app_create_page();
    app_loop();

    return 0;
}
