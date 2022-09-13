// lv_box.h
//

#ifndef __LV_BOX_H__
#define __LV_BOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef enum _lv_box_type
{
    ALTITUDE_GROUND,
    ALTITUDE_AGL,       // Height Above Ground Level
    ALTITUDE_PROFILE,
    SPEED_GROUND,
    SPEED_AIR,
    SPEED_VERTICAL,
    SPEED_VERTICAL_LAZY,
    TRACK_HEADING,
    TARCK_BEARING,
    TIME_FLIGHT,
    TIME_CURRENT,
    TIME_TO_NEXT_WAYPOINT,
    TIME_REMAIN,
    DISTANCE_TAKEOFF,
    DISTANCE_LANDING,
    DISTANCE_NEXT_WAYPOINT,
    DISTANCE_FLIGHT,    // odometer
    LIFT_vs_DRAG,
    COMPASS,
    VSPEED_BAR,
    VSPEED_PROFILE,
    TRACK_FLIGHT,
	END_OF_BOX,
    BOX_COUNT = END_OF_BOX,
    
} lv_box_type_t;


////////////////////////////////////////////////////////////////////////////
//

void 				lv_box_init();
lv_obj_t* 			lv_box_create(lv_obj_t* parent, const char* title, const char* desc);
void 				lv_box_set_title(lv_obj_t* box, const char* title);
void 				lv_box_set_description(lv_obj_t* box, const char* desc);
void				lv_box_set_content(lv_obj_t* box, lv_box_type_t type);

void                lv_box_update(lv_obj_t* box);

const char*			lv_box_get_title(lv_box_type_t type);
const char*			lv_box_get_description(lv_box_type_t type);


lv_obj_t*           lv_box_get_stock_object(lv_box_type_t type);
void                lv_box_detach_all();

#ifdef __cplusplus
}
#endif

#endif // __LV_BOX_H__
