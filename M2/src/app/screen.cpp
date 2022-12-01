// screen.cpp
//

#include "board.h"
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

    _alertObj = nullptr;
    _alertTimer = nullptr;
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

void Screen::setApplication(Application* app)
{
    if (_this)
        lv_obj_set_user_data(_this, app);
}

Application* Screen::getApplication()
{
    if (!_this)
        return nullptr;
        
    return (Application *)lv_obj_get_user_data(_this);
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

extern const lv_img_dsc_t goodbye;

void Screen::showPowerOff()
{
    lv_obj_t* bkgnd = lv_obj_create(_this);
    lv_obj_set_pos(bkgnd, 0, 0);
    lv_obj_set_size(bkgnd, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_set_style_bg_color(bkgnd, lv_color_hex(0x808080), 0);
    lv_obj_set_style_bg_opa(bkgnd, LV_OPA_60, 0);

    #if 0
    lv_obj_t* label = lv_label_create(bkgnd);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFF0FF00), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);
    lv_label_set_text(label, "Turn-off Device!!");
    #else
    lv_obj_t* img = lv_img_create(bkgnd);
    lv_img_set_src(img, &goodbye);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    #endif
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

void Screen::_onDismissAlert(lv_timer_t* timer)
{
    Screen* _this = (Screen *)timer->user_data;

    // delete alert-object
    lv_obj_del(_this->_alertObj);

    // reset
    _this->_alertObj = nullptr;
    _this->_alertTimer = nullptr;
}

void Screen::notifyMesage(const char* msg)
{
    showAlert(msg, false);
}

void Screen::notifyError(const char* err)
{
    showAlert(err, true);
}

lv_obj_t* Screen::createAlertObject(lv_color_t colorBkgnd)
{
    lv_obj_t* alert = lv_label_create(_this);

    lv_obj_set_style_bg_color(alert, colorBkgnd, 0);
    lv_obj_set_style_bg_opa(alert, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(alert, 12, 0);
    lv_obj_set_style_border_width(alert, 1, 0);
    lv_obj_set_style_border_color(alert, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_opa(alert, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(alert, 24, 0);
    lv_obj_set_style_pad_ver(alert, 6, 0);
    lv_obj_set_style_text_color(alert, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(alert, &lv_font_montserrat_14, 0);
    lv_obj_align(alert, LV_ALIGN_BOTTOM_MID, 0, -10);

    return alert;
}

void Screen::showAlert(const char* msg, bool typeError)
{
    lv_color_t bkgndColor = lv_color_hex(typeError ? 0xFFD0D0 : 0xD0D0FF);

    if (_alertObj == nullptr)
    {
        _alertObj = createAlertObject(bkgndColor);
        _alertTimer = lv_timer_create(_onDismissAlert, 2000, this);
        lv_timer_set_repeat_count(_alertTimer, 1);
    } 
    else 
    {
        lv_obj_set_style_bg_color(_alertObj, bkgndColor, 0);
        lv_timer_reset(_alertTimer);    
    }

    lv_label_set_text(_alertObj, msg);       
}
