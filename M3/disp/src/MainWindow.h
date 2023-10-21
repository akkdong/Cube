// MainWindow.h
//

#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <stdint.h>
#include <time.h>

#include "Window.h"



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

    enum PageType {
        PID_DEFAULT,
        PID_THERMAL,
        PID_COMPETITION,
        PID_COUNT
    };

    enum ValueType {
        VT_ALTITUDE,
        VT_GROUND_SPEED,
        VT_VERTICAL_SPEED,
        VT_AGL,
        VT_LD,
        VT_TRACK
    };
    
    struct LayoutInfo {
        int x, y, w, h;
        bool visible;
        uint32_t flag;
    };

    struct Layout {
        LayoutInfo widget[WID_COUNT];
        int vbTitleSize;
        int vbValueSize;
    };


    //
    virtual int update(DeviceContext *context, uint32_t updateHints);

protected:
    //
    void showMessage(const char *msg);

    void changePage(int pageNum);
    void saveScreen();

    //
    virtual void onActive();
    virtual void onDraw();   
    virtual void onMessage(uint32_t code, uint32_t data);
    virtual void onTimer(uint32_t id);

    virtual void onTouchDown(int x, int y);
    virtual void onTouchMove(int x, int y);
    virtual void onTouchUp(int x, int y);

    virtual void onKeyPressed(uint32_t key);
    virtual void onKeyLongPressed(uint32_t key);
    virtual void onKeyReleased(uint32_t key);

protected:
    Annunciator m_ann;
    ThermalAssist m_thermal;
    Compass m_compass;
    VarioMeter m_vmeter;
    ValueBox m_vbox[6];
    MessageBox m_mbox;

    Widget *m_widgets[WID_COUNT];
    Layout m_layout[PID_COUNT];
    int m_activePage;
};


#endif // __MAIN_WINDOW_H__
