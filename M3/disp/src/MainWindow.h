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

    enum ValueType {
        VT_ALTITUDE,
        VT_GROUND_SPEED,
        VT_VERTICAL_SPEED,
        VT_AGL,
        VT_LD,
        VT_TRACK
    };

    //
    virtual int update(DeviceContext *context, uint32_t updateHints);
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


#endif // __MAIN_WINDOW_H__
