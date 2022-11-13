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

        this->onCreate(parent);
        return true;
    }
    virtual void update() {}

    virtual lv_obj_t * getObject() { 
        return _this; 
    }
    virtual void setPosition(int x, int y) {
        if (_this)
            lv_obj_set_pos(_this, x, y);
    }
    virtual void setSize(int w, int h) {
        if (_this)
            lv_obj_set_size(_this, w, h);
    }

    virtual void onCreate(DisplayObject* parent) {}
    virtual void onDestroy() {}

protected:
    lv_obj_t*      _this;
};




#endif // __DISPLAY_BASE_H__
