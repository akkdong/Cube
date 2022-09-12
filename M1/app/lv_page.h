// lv_page.h
//

#ifndef __LV_PAGE_H__
#define __LV_PAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_box.h"

#define MAX_BOX_COUNT       (12)


////////////////////////////////////////////////////////////////////////////
//

typedef struct _lv_page_item
{
    lv_box_type_t type;
    lv_coord_t  x, y, w, h;
	lv_border_side_t side;

} lv_page_item_t;



////////////////////////////////////////////////////////////////////////////
//

void    lv_page_create(lv_obj_t* parent, lv_page_item_t* boxes);


#ifdef __cplusplus
}
#endif

#endif // __LV_PAGE_H__
