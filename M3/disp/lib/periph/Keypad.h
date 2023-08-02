// keypad.h
//

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <Arduino.h>
#include "KeypadInput.h"



/////////////////////////////////////////////////////////////////////////////////////
//

struct IKeypadCallback
{
    virtual void onPressed(uint8_t key) = 0;
    virtual void onLongPressed(uint8_t key) = 0;
    virtual void onReleased(uint8_t key) = 0;
};



/////////////////////////////////////////////////////////////////////////////////////
//

class Keypad
{
public:
    Keypad(IKeypadCallback* callback = NULL);

public:
    int             begin(IKeypadInput* input);
    int             update();

    void            setCallback(IKeypadCallback* callback) { keyCallback = callback; }

protected:
    IKeypadCallback *keyCallback;
    IKeypadInput    *keyInput;
};

#endif // __KEYPAD_H__
