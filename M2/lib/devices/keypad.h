// keypad.h
//

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <Arduino.h>
#include "abstract/KeypadInput.h"

#define TIME_DEBOUNCE       (100)
#define TIME_LONG_PRESS     (2000)

#define KEY_COUNT           (4)


/////////////////////////////////////////////////////////////////////////////////////
//

struct KeypadCallback
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
    Keypad(KeypadCallback* callback = NULL);


    enum KeyState {
        RELEASE,
        PRE_PRESS,
        PRESS,
        LONG_PRESS,
        POST_PRESS
    };

    struct Key
    {
        uint8_t		pin;
        uint8_t		active; // 0: ACTIVE_LOW, others: ACTIVE_HIGH
        uint8_t	    key;
        uint8_t     state;  // KeyState
        uint32_t    tick;
    };


public:
    void            begin(IKeypadInput* pKeypad);
    void            update();

    void            setCallback(KeypadCallback* callback) { this->callback = callback; }

protected:
    KeypadCallback* callback;

    IKeypadInput*   keyInput;
    Key             keyMap[KEY_COUNT];
};

#endif // __KEYPAD_H__
