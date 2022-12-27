// keypad.cpp
//

#include "Keypad.h"
#include "board.h"
#include "logger.h"
#include "utils.h"
#include "config.h"


/////////////////////////////////////////////////////////////////////////////////////
//

Keypad::Keypad(KeypadCallback* callback)
    : keyInput(NULL)
    , keyMap {
        { GPIO_KEY,                     0,  KEY_ESCAPE, RELEASE, 0 }, // active-low
        { GPIO_EXT_MASK | GPIO_EXT_IO0,	0, 	KEY_ENTER,  RELEASE, 0 }, // active-low, extended-key
        { GPIO_EXT_MASK | GPIO_EXT_IO1, 0, 	KEY_LEFT,   RELEASE, 0 }, // active-low, extended-key
        { GPIO_EXT_MASK | GPIO_EXT_IO2, 0, 	KEY_RIGHT,  RELEASE, 0 }  // active-low, extended-key
    }
    , callback(callback)
{

}

void Keypad::begin(IKeypadInput* input)
{
    keyInput = input;

    for (size_t i = 0; i < sizeof(keyMap) / sizeof(keyMap[0]); i++)
        keyMap[i].state = RELEASE;
}

void Keypad::update()
{
    // read in advance
    keyInput->PreRead();

    for (size_t i = 0; i < sizeof(keyMap) / sizeof(keyMap[0]); i++)
    {
        Key* key = &keyMap[i];
        int value = keyInput ? keyInput->Read(key->pin) : 0;
        uint8_t state = RELEASE; // release
        if ((key->active == 0 && value == 0) || (key->active != 0 && value != 0))
            state = PRESS; // press
        uint32_t tick = get_tick();

        switch (key->state)
        {
        case RELEASE:
            if (state != RELEASE)
            {
                #if 0
                key->state = PRE_PRESS;
                key->tick = tick;
                #else // NO_USE_DEBOUND
                key->state = PRESS;
                key->tick = tick;

                if (callback)
                    callback->onPressed(key->key);
                #endif
            }
            break;
        case PRE_PRESS:
            if (state != RELEASE)
            {
                if (tick - key->tick > TIME_DEBOUNCE)
                {
                    key->state = PRESS;
                    key->tick = tick;

                    if (callback)
                        callback->onPressed(key->key);
                }
            }
            else
            {
                key->state = RELEASE;
            }
            break;
        case PRESS:
        case LONG_PRESS:
            if (state == RELEASE)
            {
                key->state = RELEASE;
                key->tick = 0;

                if (callback)
                    callback->onReleased(key->key);
            }
            else if (key->state == PRESS && tick - key->tick > TIME_LONG_PRESS)
            {
                key->state = LONG_PRESS;
                key->tick = tick;

                if (callback)
                    callback->onLongPressed(key->key);
            }
            break;
        /*
        case POST_PRESS:
            if (tick - key->tick > TIME_DEBOUNCE)
            {
                key->state = RELEASE;
                key->tick = 0;

                if (callback)
                    callback->onReleased(key->key);
            }
            break;
        */
        }
    }    
}
