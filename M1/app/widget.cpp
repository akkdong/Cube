// widget.cpp
//

#include "widget.h"



////////////////////////////////////////////////////////////////////////////////////
// class Widget

lv_style_t* Widget::def_style = NULL;

const lv_font_t* Widget::font_title = &lv_font_montserrat_14;
const lv_font_t* Widget::font_subtitle = &lv_font_montserrat_12;
const lv_font_t* Widget::font_body = &lv_font_montserrat_48;


Widget::Widget()
{
    if (!def_style)
        Widget::init();
}

void Widget::init()
{
    //
    def_style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
    if (def_style != NULL)
    {
        lv_style_init(def_style);
        lv_style_set_pad_hor(def_style, 4);
        lv_style_set_pad_ver(def_style, 2);
        lv_style_set_radius(def_style, 0/*4*/);
        lv_style_set_border_width(def_style, 0/*1*/);
        lv_style_set_border_side(def_style, LV_BORDER_SIDE_NONE);
        lv_style_set_bg_color(def_style, lv_color_hex(0xFFFFFF));
    }
}

void Widget::onCreate()
{
    if (def_style)
        lv_obj_add_style(widget, def_style, 0);
}


////////////////////////////////////////////////////////////////////////////////////
// class Annunciator

Annunciator::Annunciator()
{

}

void Annunciator::create(lv_obj_t* parent)
{
    widget = lv_obj_create(parent);

    onCreate();
}

void Annunciator::update()
{

}

void Annunciator::onCreate()
{
    Widget::onCreate();

    // ...
}





class Page
{
public:
    Page();


    Widget*     widgets;

};