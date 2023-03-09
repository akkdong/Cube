// keypad.h
//

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <Arduino.h>
#include "abstract/KeypadInput.h"

#define KEY_COUNT           (4)

#define KEY_ENTER           (0xB0)
#define KEY_ESCAPE          (0xB1)
#define KEY_LEFT            (0xD8)
#define KEY_RIGHT           (0xD7)


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
    Keypad();


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
    void                begin(IKeypadInput* pKeypad, IKeypadCallback* callback = NULL);
    void                update();

public:
    void setCallback(IKeypadCallback* callback) { 
        keyCallback = callback; 
    }

protected:
    IKeypadInput*       keyInput;
    IKeypadCallback*    keyCallback;

    Key                 keyMap[KEY_COUNT];
};

#endif // __KEYPAD_H__
