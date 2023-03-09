// keypad.cpp
//

#include "Keypad.h"
#include "board.h"
#include "config.h"
#include "logger.h"
#include "utils.h"

#define TIME_DEBOUNCE       (10)
#define TIME_LONG_PRESS     (2000)



/////////////////////////////////////////////////////////////////////////////////////
//

Keypad::Keypad()
    : keyInput(NULL)
    , keyCallback(nullptr)
    , keyMap {
        { GPIO_KEY_UP,      0,  KEY_ESCAPE, RELEASE, 0 }, // active-low
        { GPIO_KEY_DOWN,	0, 	KEY_ENTER,  RELEASE, 0 }, // active-low
        { GPIO_KEY_LEFT,    0, 	KEY_LEFT,   RELEASE, 0 }, // active-low
        { GPIO_KEY_RIGHT,   0, 	KEY_RIGHT,  RELEASE, 0 }  // active-low
    }
{
}

void Keypad::begin(IKeypadInput* input, IKeypadCallback* callback)
{
    this->keyInput = input;
    this->keyCallback = callback;

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

                if (keyCallback)
                    keyCallback->onPressed(key->key);
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

                    if (keyCallback)
                        keyCallback->onPressed(key->key);
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

                if (keyCallback)
                    keyCallback->onReleased(key->key);
            }
            else if (key->state == PRESS && tick - key->tick > TIME_LONG_PRESS)
            {
                key->state = LONG_PRESS;
                key->tick = tick;

                if (keyCallback)
                    keyCallback->onLongPressed(key->key);
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
