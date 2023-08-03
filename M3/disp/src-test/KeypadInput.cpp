// KeypadInput.cpp
//

#include <Arduino.h>
#include "board.h"
#include "KeypadInput.h"

#define KEY_COUNT       (3)


///////////////////////////////////////////////////////////////////////////////////////////////
//

class KeypadInput : public IKeypadInput
{
public:
    KeypadInput() 
        : keyIndex(0)
        , keyMap {
            { KEY_LEFT,     0,  KEY_LEFT,   RELEASE, 0 }, // active-low
            { KEY_PUSH,	    0, 	KEY_PUSH,   RELEASE, 0 }, // active-low
            { KEY_RIGHT,    0, 	KEY_RIGHT,  RELEASE, 0 }, // active-low
        }
    {

    }

    void begin() 
    {
        for (size_t i = 0; i < sizeof(keyMap) / sizeof(keyMap[0]); i++)
            keyMap[i].state = RELEASE;
    }

    Key* getFirstKey() 
    {
        keyIndex = 0;

        return &keyMap[keyIndex++];
    }

    Key* getNextKey() 
    {
        if (keyIndex >= sizeof(keyMap) / sizeof(keyMap[0]))
            return nullptr;

        return &keyMap[keyIndex++];
    }

    void preRead() 
    {
    }

    int read(uint8_t pin) 
    {
        return digitalRead(pin);
    }

protected:
    uint8_t keyIndex;
    Key     keyMap[KEY_COUNT];
};


/////////////////////////////////////////////////////////////////////////////////////
//

static KeypadInput _input;

IKeypadInput* CreateKeypadInput()
{
    return &_input;
}