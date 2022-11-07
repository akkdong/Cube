// KeypadInput.cpp
//

#include <Arduino.h>

#include "abstract/KeypadInput.h"
#include "board.h"
#include "tca9554.h"


///////////////////////////////////////////////////////////////////////////////////////////////
//

class KeypadInput : public IKeypadInput
{
public:
    KeypadInput() {}

    virtual int Read(uint8_t pin) {
        if (pin & GPIO_EXT_MASK)
        {
            uint8_t input = exio.getInput();
            return (input & (1 << (pin & (~GPIO_EXT_MASK)))) ? 1 : 0;
        }
        
        return digitalRead(pin);
    }
};


/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput()
{
    static KeypadInput _input;

    return &_input;
}