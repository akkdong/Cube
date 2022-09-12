// lv_page.h
//

#include "lv_page.h"



void lv_page_create(lv_obj_t* parent, lv_page_item_t* boxes)
{
    // detach all boxes (hide-all)
    lv_box_hide_all();

    // attach used-boxes to page (resize, position & show)
    for (int i = 0; boxes[i].type != END_OF_BOX; i++)
    {
        lv_box_type_t type = boxes[i].type;
        /*
        lv_obj_t* box = lv_box_create(parent, lv_box_get_title(type), lv_box_get_description(type));        
        */
        lv_obj_t* box = lv_box_get_stock_object(type);
        if (box)
        {
            lv_obj_set_parent(box, parent);
            lv_obj_set_pos(box, boxes[i].x + 1, boxes[i].y + 1);
            lv_obj_set_size(box, boxes[i].w - 2, boxes[i].h - 2);
            //lv_obj_set_style_border_side(box, boxes[i].side, 0);

            lv_box_set_content(box, type);
        }
    }
}
