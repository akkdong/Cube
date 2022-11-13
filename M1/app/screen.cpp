// screen.cpp
//

#include "device_defines.h"
#include "screen.h"


///////////////////////////////////////////////////////////////////////////////////
// class Window

Window::Window()
{

}

Window::~Window()
{
    if (_this)
        lv_obj_del(_this);
}


bool Window::create(DisplayObject* parent)
{
    if (!DisplayObject::create(parent))
        return false;

    lv_obj_set_pos(_this, 0, 0);
    lv_obj_set_size(_this, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_set_style_pad_hor(_this, 0, 0);
    lv_obj_set_style_pad_ver(_this, 0, 0);
    lv_obj_set_style_radius(_this, 0, 0);
    lv_obj_set_style_border_width(_this, 0, 0);

    return true;
}


///////////////////////////////////////////////////////////////////////////////////
// class Screen

Screen::Screen()
{
    memset(_stack, 0, sizeof(_stack));
    _top = 0;
}


bool Screen::create(DisplayObject* /*parent*/)
{
    if (!Window::create(NULL))
        return false;

    lv_obj_set_style_bg_color(_this, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_bg_opa(_this, LV_OPA_COVER, 0);
    lv_scr_load(_this);

    return true;
}

void Screen::activateWindow(Window* window) 
{
    // create display object
    if (!window->create(this))
        return;

    // push to window-stack
    pushWindow(window);

    // foreground & activate it
    lv_obj_move_foreground(window->getObject());
    lv_async_call(_onActivate, window);
}

void Screen::fallbackWindow(Result result) 
{
    // close foreground-window
    Window* window = popWindow();
    if (!window)
        return;
    window->onClose(/*result*/);

    //
    lv_async_call(_onCleanup, window);

    // re-activate next-window
    Window* next = peekWindow();
    if (next)
        next->onFallback(result, window);
}

void Screen::switchWindow(Window* window) 
{
    fallbackWindow(RES_OK);
    activateWindow(window);
}

Screen* Screen::instance() 
{
    static Screen  screen;

    if (!screen.getObject())
    {
        if (!screen.create(NULL))
            return NULL;
    }

    return &screen;
}

void Screen::pushWindow(Window* window)
{
    if (_top < sizeof(_stack))
        _stack[_top++] = window;
}

Window* Screen::peekWindow()
{
    if (_top > 0)
        return _stack[_top - 1];

    return NULL;
}

Window* Screen::popWindow()
{
    Window* window = NULL;

    if (_top > 0)
        window = _stack[--_top];

    return window;
}


void Screen::_onActivate(void* userData) 
{
    Window* window = (Window *)userData;
    window->onActive();
}

void Screen::_onCleanup(void* userData) 
{
    Window* window = (Window *)userData;
    delete window;
}
