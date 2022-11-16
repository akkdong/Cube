// settingWindow.h
//

#ifndef __SETTING_WINDOW_H__
#define __SETTING_WINDOW_H__

#include "screen.h"
#include "widget.h"

///////////////////////////////////////////////////////////////////////////////////
// class SettingWindow

class SettingWindow : public Window
{
public:
    SettingWindow();
    virtual ~SettingWindow();

public:
    //
    void            update() override;

    //
    void            onCreate() override;
    void            onActive() override;
    void            onClose() override;

    void            onKeyDown(uint16_t key) override;
    void            onKeyLongDown(uint16_t key) override;
    void            onKeyUp(uint16_t key) override;

protected:
    lv_obj_t*       createObject(lv_obj_t* parent) override;

protected:

};

#endif // __SETTING_WINDOW_H__
