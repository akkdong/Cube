// Keypad.cpp
//

#include <stdint.h>
#include "device_defines.h"
#include "logger.h"
#include "utils.h"

#include "Keypad.h"


/////////////////////////////////////////////////////////////////////////////////////
//

Keypad::Keypad(KeypadCallback* callback)
    : keyInput(NULL)
    , keyMap {
        { BTN_SEL, 		0, 	KEY_RETURN,         RELEASE },
        { BTN_LEFT, 	1, 	KEY_LEFT_ARROW,     RELEASE },
        { BTN_RIGHT, 	1, 	KEY_RIGHT_ARROW,    RELEASE },
        { BTN_UP, 		1, 	KEY_UP_ARROW,       RELEASE },
        { BTN_DOWN, 	1, 	KEY_DOWN_ARROW,     RELEASE },
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
                key->state = PRE_PRESS;
                key->tick = tick;
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
                        callback->OnPressed(key->key);
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
                key->state = POST_PRESS;
                key->tick = tick;
            }
            else if (key->state == PRESS && tick - key->tick > TIME_LONG_PRESS)
            {
                key->state = LONG_PRESS;
                key->tick = tick;

                if (callback)
                    callback->OnLongPressed(key->key);
            }
            break;
        case POST_PRESS:
            if (tick - key->tick > TIME_DEBOUNCE)
            {
                key->state = RELEASE;
                key->tick = 0;

                if (callback)
                    callback->OnReleased(key->key);
            }
            break;
        }
    }    
}
