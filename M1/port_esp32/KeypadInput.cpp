// KeypadInput.cpp
//

#include <Arduino.h>

#include "abstract/KeypadInput.h"


///////////////////////////////////////////////////////////////////////////////////////////////
//

class KeypadInput : public IKeypadInput
{
public:
    KeypadInput() {}

    virtual int Read(uint8_t pin) {
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