// lv_page.h
//

#include "lv_page.h"



void lv_page_create(lv_obj_t* page, lv_page_item_t* boxes)
{
    // detach all boxes (hide-all)
    lv_box_detach_all();

    // attach used-boxes to page (resize, position & show)
    for (int i = 0; boxes[i].type != END_OF_BOX; i++)
    {
        lv_box_type_t type = boxes[i].type;
        /*
        lv_obj_t* box = lv_box_create(page, lv_box_get_title(type), lv_box_get_description(type));        
        */
        lv_obj_t* box = lv_box_get_stock_object(type);
        if (box)
        {
            lv_obj_set_parent(box, page);
            lv_obj_set_pos(box, boxes[i].x + 1, boxes[i].y + 1);
            lv_obj_set_size(box, boxes[i].w - 2, boxes[i].h - 2);
            //lv_obj_set_style_border_side(box, boxes[i].side, 0);

            lv_box_set_content(box, type);
        }
    }
}

void lv_page_update(lv_obj_t* page)
{
    for (int i = 0; i < lv_obj_get_child_cnt(page); i++)
    {
        lv_obj_t* box = lv_obj_get_child(page, i);
        lv_box_update(box);
    }
}
