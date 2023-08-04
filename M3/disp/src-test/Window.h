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

protected:
};



/////////////////////////////////////////////////////////////////////////////////
// class MainWindow

class MainWindow : public Window
{
public:
    MainWindow(M5EPD_Canvas *pRefCanvas);

    enum WidgetType {
        WID_ANNUNCIATOR,
        WID_THERMALASSIST,
        WID_COMPASS,
        WID_VARIOMETER,
        WID_VALUEBOX_1,
        WID_VALUEBOX_2,
        WID_VALUEBOX_3,
        WID_VALUEBOX_4,
        WID_VALUEBOX_5,
        WID_VALUEBOX_6,
        WID_COUNT
    };

    enum ValueType {
        VT_ALTITUDE,
        VT_GROUND_SPEED,
        VT_VERTICAL_SPEED,
        VT_AGL,
        VT_LD,
        VT_TRACK
    };

    //
    virtual int update(DeviceContext *context);
    virtual void draw();

    //
    virtual void onActive();

    

protected:
    Annunciator m_ann;
    ThermalAssist m_thermal;
    Compass m_compass;
    VarioMeter m_vmeter;
    ValueBox m_vbox[6];

    Widget *m_widgets[WID_COUNT];
};

#endif // __WINDOW_H__
