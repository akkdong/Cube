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
        WIDGET_BOX_10,
        WIDGET_BOX_11,
        WIDGET_BOX_12,
        WIDGET_PROFILE_VARIO,
        WIDGET_PROFILE_ALTITUDE,
        WIDGET_COMPASS,
        WIDGET_VARIOMETER,
        WIDGET_THERMAL_ASSISTANT,
        WIDGET_COUNT
    };

public:
    //
    void            update() override;

    void            onCreate() override;
    void            onActive() override;
    void            onFallback(Result result, Window* from) override;
    void            onClose() override;

    void            onKeyDown(uint16_t key) override;
    void            onLongKeyDown(uint16_t key) override;
    void            onKeyUp(uint16_t key) override;    

protected:
    Widget*         createWidget(Widget::Type type, DisplayObject* parent);
    void            layoutWidget(int layout);

    const char *    getElapsedTimeString(char* str, time_t t);
    const char *    getTimeString(char* str, time_t t, bool includeSecond = false);

    //
    void            onUpdate(Annunciator *);
    void            onUpdate(NumberBox *);
    void            onUpdate(ProfileWidget *);
    void            onUpdate(CompassWidget *);
    void            onUpdate(VariometerWidget *);
    void            onUpdate(ThermalAssistant *); 

    //
    static bool     getCustomFont(const lv_font_t * font, void * img_src, uint16_t len, uint32_t unicode, uint32_t unicode_next);

    static void     _onClickBackground(lv_event_t* event);

protected:
    Annunciator     annunciator;
    CanvasWidget    bkgndCanvas;
    Widget*         widgets[WIDGET_COUNT];
    int             activeLayout;

    lv_font_t*      fontCustom;

    static const Widget::Type widgetID2Type[WIDGET_COUNT]; // NumberBox(9), Profile(2), Compass, Variometer, Thermal-assistant
};

#endif // __FLIGHT_WINDOW_H__
