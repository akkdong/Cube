// displayBase.h
//

#ifndef __DISPLAY_BASE_H__
#define __DISPLAY_BASE_H__

#ifdef ARDUINO
#include <Arduino.h>
#else

#endif

#include "lvgl.h"


// display-object
//      window  // full-screen
//          screen
//          startup-window
//          flight-window
//          menu-navigator
//          task-editor
//          sound-editor
//          setting
//          mesage-box
//      widget  // partial-screen 
//          canvas-widget
//          canvas-virtual

// display-object
//      lv_obj_t *
//      position
//      size
//      preference


///////////////////////////////////////////////////////////////////////////////////
// class DisplayObject

class DisplayObject
{
public:
    DisplayObject() : _this(NULL) {}
    virtual ~DisplayObject() {}

public:
    virtual bool create(DisplayObject* parent) {
        _this = lv_obj_create(parent ? parent->getObject() : NULL);
        if (!_this)
            return false;

        // Should I call onCreate asynchrously or directly?
        #if 0
        lv_async_call(_onCreate, this);
        #else
        this->onCreate();
        #endif

        return true;
    }
    virtual void update() {}

    virtual lv_obj_t * getObject() { 
        return _this; 
    }
    virtual bool getVisible() {
        if (!_this)
            return false;

        return !lv_obj_has_flag(_this, LV_OBJ_FLAG_HIDDEN);
    }
    virtual void setPosition(int x, int y) {
        if (_this)
            lv_obj_set_pos(_this, x, y);
    }
    virtual void setSize(int w, int h) {
        if (_this)
            lv_obj_set_size(_this, w, h);
    }
    virtual void setVisible(bool show) {
        if (_this) {
            if (show)
                lv_obj_clear_flag(_this, LV_OBJ_FLAG_HIDDEN);
            else
                lv_obj_add_flag(_this, LV_OBJ_FLAG_HIDDEN);
        }
    }

    virtual void onCreate() {}
    virtual void onDestroy() {}

protected:
    static void _onCreate(void* userData) {
        ((DisplayObject *)userData)->onCreate();
    }

protected:
    lv_obj_t*      _this;
};




#endif // __DISPLAY_BASE_H__
