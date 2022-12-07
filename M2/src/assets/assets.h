// assets.h
//

#ifndef __app_assets_h__
#define __app_assets_h__

#include "lvgl.h"

#define APP_ICON_BATTERY_CHARGING       0xF001
#define APP_ICON_BATTERY_0              0xF002
#define APP_ICON_BATTERY_1              0xF003
#define APP_ICON_BATTERY_2              0xF004
#define APP_ICON_BATTERY_3              0xF005
#define APP_ICON_BATTERY_4              0xF006

#define APP_ICON_BLUETOOTH_DISABLED     0xF011
#define APP_ICON_BLUETOOTH_UNPAIRED     0xF012
#define APP_ICON_BLUETOOTH_PAIRED       0xF013

#define APP_ICON_GPS_FIXED              0xF021
#define APP_ICON_GPS_UNFIXED            0xF022

#define APP_ICON_LOGO                   0xF031
#define APP_ICON_LOGO_ANGRY_A           0xF032
#define APP_ICON_LOGO_ANGRY_B           0xF033
#define APP_ICON_LOGO_O                 0xF034
#define APP_ICON_LOGO_ROUND             0xF035

#define APP_ICON_SDCARD_READY           0xF041
#define APP_ICON_SDCARD_LOGGING         0xF042

#define APP_ICON_VOLUME_MUTE            0xF051
#define APP_ICON_VOLUME_LOW             0xF052
#define APP_ICON_VOLUME_HIGH            0xF053

#define APP_SYMBOL_BATTERY_CHARGING     "\xEF\x80\x81"
#define APP_SYMBOL_BATTERY_0            "\xEF\x80\x82"
#define APP_SYMBOL_BATTERY_1            "\xEF\x80\x83"
#define APP_SYMBOL_BATTERY_2            "\xEF\x80\x84"
#define APP_SYMBOL_BATTERY_3            "\xEF\x80\x85"
#define APP_SYMBOL_BATTERY_4            "\xEF\x80\x86"

#define APP_SYMBOL_BLUETOOTH_DISABLED   "\xEF\x80\x91"
#define APP_SYMBOL_BLUETOOTH_UNPAIRED   "\xEF\x80\x92"
#define APP_SYMBOL_BLUETOOTH_PAIRED     "\xEF\x80\x93"

#define APP_SYMBOL_GPS_FIXED            "\xEF\x80\xA1"
#define APP_SYMBOL_GPS_UNFIXED          "\xEF\x80\xA2"

#define APP_SYMBOL_LOGO                 "\xEF\x80\xB1"
#define APP_SYMBOL_LOGO_ANGRY_A         "\xEF\x80\xB2"
#define APP_SYMBOL_LOGO_ANGRY_B         "\xEF\x80\xB3"
#define APP_SYMBOL_LOGO_O               "\xEF\x80\xB4"
#define APP_SYMBOL_LOGO_ROUND           "\xEF\x80\xB5"

#define APP_SYMBOL_SDCARD_READY         "\xEF\x81\x81"
#define APP_SYMBOL_SDCARD_LOGGING       "\xEF\x81\x82"

#define APP_SYMBOL_VOLUME_MUTE          "\xEF\x81\x91"
#define APP_SYMBOL_VOLUME_LOW           "\xEF\x81\x92"
#define APP_SYMBOL_VOLUME_HIGH          "\xEF\x81\x93"


// icon 16
LV_IMG_DECLARE(paper_plane);
LV_IMG_DECLARE(bluetooth);
LV_IMG_DECLARE(map_marker);
LV_IMG_DECLARE(volume);
LV_IMG_DECLARE(letter_n_16);
// icon 24
LV_IMG_DECLARE(letter_n);
// icon 32
LV_IMG_DECLARE(battery_charging);
LV_IMG_DECLARE(battery_0);
LV_IMG_DECLARE(battery_1);
LV_IMG_DECLARE(battery_2);
LV_IMG_DECLARE(battery_3);
LV_IMG_DECLARE(battery_4);

LV_IMG_DECLARE(bluetooth_disabled);
LV_IMG_DECLARE(bluetooth_unpaired);
LV_IMG_DECLARE(bluetooth_paired);

LV_IMG_DECLARE(gps_fixed);
LV_IMG_DECLARE(gps_unfixed);

LV_IMG_DECLARE(logo);
LV_IMG_DECLARE(logo_angry_a);
LV_IMG_DECLARE(logo_angry_b);
LV_IMG_DECLARE(logo_o);
LV_IMG_DECLARE(logo_round);

LV_IMG_DECLARE(sdcard_logging);
LV_IMG_DECLARE(sdcard_ready);

LV_IMG_DECLARE(volume_high);
LV_IMG_DECLARE(volume_low);
LV_IMG_DECLARE(volume_mute);

// images
LV_IMG_DECLARE(startup);
LV_IMG_DECLARE(goodbye);

LV_IMG_DECLARE(startup_a);
LV_IMG_DECLARE(startup_b);
LV_IMG_DECLARE(startup_c);


#endif // __app_assets_h__
