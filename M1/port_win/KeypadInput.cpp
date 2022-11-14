// KeypadInput.cpp
//

#include <stdint.h>
#include <SDL2/SDL.h>

#include "abstract/KeypadInput.h"
#include "device_defines.h"
#include "logger.h"



///////////////////////////////////////////////////////////////////////////////////////////////
//

class KeypadInput : public IKeypadInput
{
public:
    KeypadInput() : keyState(0) {}

    enum KeyValue {
        BIT_ENTER = (1 << 0),
        BIT_LEFT = (1 << 1),
        BIT_RIGHT = (1 << 2),
        BIT_UP = (1 << 3),
        BIT_DOWN = (1 << 4)
    };

    virtual int Read(uint8_t pin) {
        int state = 0;
        switch (pin)
        {
        case BTN_SEL:
            state = (keyState & BIT_ENTER) ? 1 : 0;
            break;
        case BTN_LEFT:
            state = (keyState & BIT_LEFT) ? 1 : 0;
            break;
        case BTN_RIGHT:
            state = (keyState & BIT_RIGHT) ? 1 : 0;
            break;
        case BTN_UP:
            state = (keyState & BIT_UP) ? 1 : 0;
            break;
        case BTN_DOWN:
            state = (keyState & BIT_DOWN) ? 1 : 0;
            break;
        }

        if (pin == BTN_SEL) // active-low
            state = 1 - state;

        return state;

    }

    void KeyProc() {
        SDL_Event event;
        bool quit = false;

        LOGv("KeypadInput::KeyProc() begin");
        while (!quit)
        {
            while (SDL_PollEvent(&event)) {
                switch (event.type)
                {
                case SDL_KEYDOWN:
                    //LOGv("KeyDown: %d", event.key.keysym.scancode);
                    switch (event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_LEFT:
                        keyState |= BIT_LEFT;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        keyState |= BIT_RIGHT;
                        break;
                    case SDL_SCANCODE_UP:
                        keyState |= BIT_UP;
                        break;
                    case SDL_SCANCODE_DOWN:
                        keyState |= BIT_DOWN;
                        break;
                    case SDL_SCANCODE_RETURN:
                        keyState |= BIT_ENTER;
                        break;
                    }
                    break;
                case SDL_KEYUP:
                    //LOGv("KeyUp: %d", event.key.keysym.scancode);
                    switch (event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_LEFT:
                        keyState &= ~BIT_LEFT;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        keyState &= ~BIT_RIGHT;
                        break;
                    case SDL_SCANCODE_UP:
                        keyState &= ~BIT_UP;
                        break;
                    case SDL_SCANCODE_DOWN:
                        keyState &= ~BIT_DOWN;
                        break;
                    case SDL_SCANCODE_RETURN:
                        keyState &= ~BIT_ENTER;
                        break;
                    }
                    break;

                case SDL_QUIT:
                    quit = true;
                    break;
                }
            }
        }

        LOGv("KeypadInput::KeyProc() end");
    }

    static int ThreadProc(void* data) {
        ((KeypadInput *)data)->KeyProc();
        return 0;
    }

protected:
    uint32_t        keyState;
};


/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput()
{
    static KeypadInput _input;

    SDL_CreateThread(KeypadInput::ThreadProc, "KeyProc", (void *)&_input);

    return &_input;
}