// settingWindow.cpp
//

#include <stdint.h>
#include "device_defines.h"
#include "settingWindow.h"


///////////////////////////////////////////////////////////////////////////////////
// class SettingWindow


SettingWindow::SettingWindow()
{

}

SettingWindow::~SettingWindow()
{

}

void SettingWindow::update()
{

}

//
void SettingWindow::onCreate()
{

}

void SettingWindow::onActive()
{

}

void SettingWindow::onClose()
{

}

void SettingWindow::onKeyDown(uint16_t key)
{
    if (key == KEY_ENTER)
    {
        Screen::instance()->fallbackWindow(Window::RES_OK);
    }
}

void SettingWindow::onKeyLongDown(uint16_t key)
{

}

void SettingWindow::onKeyUp(uint16_t key)
{

}


lv_obj_t* SettingWindow::createObject(lv_obj_t* parent)
{
    lv_obj_t* tabView = lv_tabview_create(parent, LV_DIR_LEFT, 120);

    lv_obj_t* tab1 = lv_tabview_add_tab(tabView, "Setting");
    lv_obj_t* tab2 = lv_tabview_add_tab(tabView, "Tone Editor");
    lv_obj_t* tab3 = lv_tabview_add_tab(tabView, "Task Editor");
    lv_obj_t* tab4 = lv_tabview_add_tab(tabView, "Information");

    lv_obj_t* label = lv_label_create(tab1);
    lv_label_set_text(label, "Setting");

    label = lv_label_create(tab2);
    lv_label_set_text(label, "Tone");

    label = lv_label_create(tab3);
    lv_label_set_text(label, "Task");

    label = lv_label_create(tab4);
    lv_label_set_text(label, "Infomation");

    return tabView;
}