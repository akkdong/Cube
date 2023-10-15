// RouteSettingWindow.h
//

#ifndef __ROUTE_SETTING_WINDOW_H__
#define __ROUTE_SETTING_WINDOW_H__

#include <stdint.h>
#include "Window.h"
#include "Route.h"


/////////////////////////////////////////////////////////////////////////////////
// class RouteSettingWindow

class RouteSettingWindow : public Window
{
public:
    RouteSettingWindow(M5EPD_Canvas *pRefCanvas);

public:
    //

protected:
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

    //
    bool loadXcTask(const char *path);

    void recalcLayout();
    void findBoundary(XcTask *task, Math::real *border);

    void drawCircle(int cx, int cy, Math::real lat, Math::real lon, Math::real radius);
    void drawLine(int cx, int cy, Math::real lat1, Math::real lon1, Math::real lat2, Math::real lon2);
    void drawPilot(int cx, int cy, Math::real lat, Math::real lon);

protected:
    std::shared_ptr<XcTask> taskPtr;

    XcBasePoint centerPos;
    Math::real boundary[4]; // [N, E, S, W]

    Math::real zoomRatio;
    int offsetX, offsetY;
};

#endif // __ROUTE_SETTING_WINDOW_H__
