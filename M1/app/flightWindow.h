// flightWindow.h
//

#ifndef __FLIGHT_WINDOW_H__
#define __FLIGHT_WINDOW_H__

#include "screen.h"
#include "widget.h"


///////////////////////////////////////////////////////////////////////////////////
// class FlightWindow

class FlightWindow : public Window, public WidgetUpdater
{
public:
    FlightWindow();
    virtual ~FlightWindow();

    enum WidgetID {
        WIDGET_BOX_1,
        WIDGET_BOX_2,
        WIDGET_BOX_3,
        WIDGET_BOX_4,
        WIDGET_BOX_5,
        WIDGET_BOX_6,
        WIDGET_BOX_7,
        WIDGET_BOX_8,
        WIDGET_BOX_9,
        WIDGET_COMPASS,
        WIDGET_VARIOMETER,
        WIDGET_PROFILE_VARIO,
        WIDGET_PROFILE_ALTITUDE,
        WIDGET_THERMAL_ASSISTANT,
        WIDGET_COUNT
    };

public:
    virtual void    update();

    virtual void    onCreate(DisplayObject* parent);

    virtual void    onActive();
    virtual void    onClose();

protected:
    Widget*         createWidget(Widget::Type type);

    //
    void            onUpdate(Annunciator *);
    void            onUpdate(NumberBox *);
    void            onUpdate(CompassWidget *);
    void            onUpdate(VariometerWidget *);
    void            onUpdate(VarioProfile *);
    void            onUpdate(AltitudeProfile *);
    void            onUpdate(ThermalAssistant *); 

    //
    static bool     getCustomFont(const lv_font_t * font, void * img_src, uint16_t len, uint32_t unicode, uint32_t unicode_next);

protected:
    Annunciator     annunciator;
    CanvasWidget    bkgndCanvas;
    Widget*         widgets[WIDGET_COUNT]; 

    lv_font_t*      fontCustom;

    static const Widget::Type widgetType[WIDGET_COUNT]; // NumberBox(9), Compass, Variometer, Profile(vario), Profile(altitude), Thermal-assistant
};

#endif // __FLIGHT_WINDOW_H__
