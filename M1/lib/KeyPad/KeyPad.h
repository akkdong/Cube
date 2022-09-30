// KeyPad.h
//

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#define TIME_DEBOUNCE       (100)
#define TIME_LONG_PRESS     (2000)


/////////////////////////////////////////////////////////////////////////////////////
//

struct KeypadCallback
{
    virtual void OnPressed(uint8_t key) = 0;
    virtual void OnLongPressed(uint8_t key) = 0;
    virtual void OnReleased(uint8_t key) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class KeyPad
{
public:
    KeyPad(KeypadCallback* callback = NULL);


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
    void            begin();
    void            update();

    void            setCallback(KeypadCallback* callback) { this->callback = callback; }
protected:
    Key             keyMap[5];
    KeypadCallback* callback;
};

#endif // __KEYPAD_H__
