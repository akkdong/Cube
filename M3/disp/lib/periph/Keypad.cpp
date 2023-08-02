// keypad.cpp
//

#include <Arduino.h>
#include "Keypad.h"

#define TIME_DEBOUNCE       (10)
#define TIME_LONG_PRESS     (2000)



/////////////////////////////////////////////////////////////////////////////////////
//

Keypad::Keypad(IKeypadCallback* callback)
    : keyCallback(callback)
    , keyInput(nullptr)
{

}

int Keypad::begin(IKeypadInput* input)
{
    // save keypad input interface
    keyInput = input;

    // initialize keypad
    keyInput->begin();

    return 0;
}

int Keypad::update()
{
    if (!keyInput)
        return -1;

    // read in advance
    keyInput->preRead();

    Key *key = keyInput->getFirstKey();
    while (key != nullptr)
    {
        int value = keyInput->read(key->pin);

        uint8_t state = RELEASE; // release
        if ((key->active == 0 && value == 0) || (key->active != 0 && value != 0))
            state = PRESS; // press
        uint32_t tick = millis(); // get_tick();

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

        key = keyInput->getNextKey();
    }

    return 0;
}
