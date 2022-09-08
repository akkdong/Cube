// lv_app.h
//

#ifndef __LV_APP_H__
#define __LV_APP_H__

typedef struct app_conf
{
    uint8_t code;
} app_conf_t;



void app_config_init(app_conf_t* conf);
void app_init();

#endif // __LV_APP_H__
