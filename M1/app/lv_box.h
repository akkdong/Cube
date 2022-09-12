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
    
} lv_box_type_t;

typedef struct _lv_box_group_item
{
    lv_box_type_t type;
    lv_coord_t  x, y, w, h;
	lv_border_side_t side;

} lv_box_group_item_t;


typedef enum _box_content_type
{
	UNSIGNED_INT,
	SIGNED_INT,
	FLAOT,
	CANVAS,
	
} box_content_type_t;

typedef struct _box_content
{
	box_content_type_t type; // unsigned_int, signed_int, float, canvas
	union {
		uint32_t 	uValue;
		int32_t 	iValue;
		float 		fValue;
		lv_obj_t*	canvas;
	};

} box_content_t;


////////////////////////////////////////////////////////////////////////////
//

void 				lv_box_init();
lv_obj_t* 			lv_box_create(lv_obj_t* parent, const char* title, const char* desc);
void 				lv_box_set_title(lv_obj_t* box, const char* title);
void 				lv_box_set_description(lv_obj_t* box, const char* desc);
void				lv_box_set_content(box_content_t* content);

const char*			lv_box_get_title(lv_box_type_t type);
const char*			lv_box_get_description(lv_box_type_t type);


#ifdef __cplusplus
}
#endif

#endif // __LV_BOX_H__
