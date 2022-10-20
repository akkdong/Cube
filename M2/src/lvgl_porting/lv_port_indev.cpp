// lv_port_indev.cpp
//

#include "lv_port_indev.h"

#include "esp_err.h"
#include "esp_log.h"


static const char * TAG = "lv_port_indev";
lv_indev_t *        indev_touchpad;

FT5x06              touch;


///////////////////////////////////////////////////////////////////////////////////////////////////
//

static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    if (touch.getPosition(&last_x, &last_y))
        data->state = LV_INDEV_STATE_PR;
    else
        data->state = LV_INDEV_STATE_REL;

    data->point.x = last_x;
    data->point.y = last_y;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    // touch
    touch.begin();

    // register a touchpad input device
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;

    indev_touchpad = lv_indev_drv_register(&indev_drv);
}
