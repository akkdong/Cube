// Window.h
//

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Widgets.h"

enum FallbackResult {
    RES_NONE = 0,
    RES_CANCEL,
    RES_NO,
    RES_YES,
    RES_OK = RES_YES,
    RES_RETRY,
};


/////////////////////////////////////////////////////////////////////////////////
// class Window

class Window : public Widget
{
public:
    // constructor & destructor
    //Window();
    Window(M5EPD_Canvas* pRefCanvas);
    Window(M5EPD_Canvas* pRefCanvas, int x, int y, int w, int h);
 

    //
    #if OBSOLETE
    void setWindowPos(int x, int y);
    void move(int x, int y, int w, int h);
    void show(bool show);
    #endif

    //
    #if OBSOLETE
    virtual int update(DeviceContext *context) {}
    virtual void draw() {}
    //
    virtual void onDraw() {}    
    //
    virtual void onTouchDown(int x, int y) {}
    virtual void onTouchMove(int x, int y) {}
    virtual void onTouchUp(int x, int y) {}
    //
    virtual void onKeyPress(unsigned short key) {}
    virtual void onKeyLongPress(unsigned short key) {}
    virtual void onKeyRelease(unsigned short key) {}
    #endif

    //
    virtual void onActive() {}
    virtual void onClose() {}
    virtual void onFallback(FallbackResult result, Window *from) {}

    virtual void onMessage(uint16_t code, uint16_t data) {}
    virtual void onTimer(uint32_t tickCount) {}

protected:
};


#endif // __WINDOW_H__
