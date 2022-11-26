// screen.h
//

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "displayBase.h"

#define MAX_WINDOW_STACK          (5)


///////////////////////////////////////////////////////////////////////////////////
//

class Application;



///////////////////////////////////////////////////////////////////////////////////
// class Window

class Window : public DisplayObject
{
public:
    Window();
    virtual ~Window();

    enum Result {
        RES_NONE = 0,
        RES_CANCEL,
        RES_NO,
        RES_YES,
        RES_OK = RES_YES,
        RES_RETRY,
    };

public:
    virtual bool    create(DisplayObject* parent);

    virtual void    onActive() {}
    virtual void    onClose() {}
    virtual void    onFallback(Result result, Window* from) {}

    virtual void    onKeyDown(uint16_t key) {};
    virtual void    onLongKeyDown(uint16_t key) {};
    virtual void    onKeyUp(uint16_t key) {};
};



///////////////////////////////////////////////////////////////////////////////////
// class Screen

class Screen : public Window
{
public:
    Screen();

protected:
    virtual bool    create(DisplayObject* parent);

public:
    void            setApplication(Application* app);
    Application*    getApplication();

    void            activateWindow(Window* window);
    void            fallbackWindow(Result result);

    void            switchWindow(Window* window);

    void            showPowerOff();

    static Screen*  instance();
    Window*         peekWindow();

protected:
    void            pushWindow(Window* window);
    Window*         popWindow();

protected:
    static void     _onActivate(void* userData);
    static void     _onCleanup(void* userData);

protected:
    Window*         _stack[MAX_WINDOW_STACK];
    int             _top;
};


#endif // __SCREEN_H__