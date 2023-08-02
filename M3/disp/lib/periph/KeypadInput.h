// KeypadInput.h
//

#ifndef __KEYPAD_ABSTRACTION_H__
#define __KEYPAD_ABSTRACTION_H__

/////////////////////////////////////////////////////////////////////////////////////
//

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

struct IKeypadInput
{
    virtual void begin() = 0;

    virtual Key* getFirstKey() = 0;
    virtual Key* getNextKey() = 0;

    virtual void preRead() = 0;
    virtual int read(uint8_t pin) = 0;
};



/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput();


#endif // __KEYPAD_ABSTRACTION_H__
