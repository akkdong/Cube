// TopMenuWindow.h
//

#ifndef __TOP_MENU_WINDOW_H__
#define __TOP_MENU_WINDOW_H__

#include <stdint.h>
#include "Window.h"


/////////////////////////////////////////////////////////////////////////////////
// class TopMenuWindow

class TopMenuWindow : public Window
{
public:
    TopMenuWindow(M5EPD_Canvas *pRefCanvas);

    enum MenuType {
        MENU_WIFI,
        MENU_ROUTE,
        MENU_SETTING,
        MENU_POWER,
        MENU_COUNT,
    };

    struct LayoutInfo {
        int x, y, w, h;
        const unsigned char *image;
        int iw, ih;
        const char *label;
    };    

public:

protected:
    int hitTest(int x, int y);

    void activateButton(int index, bool active);
    bool moveSelection(int offset);
    void redraw();
    void redrawFast();
    
    void exec(int index);

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
    ButtonClass m_btn[MENU_COUNT];
    LayoutInfo m_layout[MENU_COUNT];

    uint32_t m_lastKey;
    int m_active;
    uint32_t m_lastTouch;
    uint32_t m_drawCount;
};



#endif // __TOP_MENU_WINDOW_H__
