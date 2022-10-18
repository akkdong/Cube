// KeypadInput.cpp
//

#include <stdint.h>

#include "abstract/KeypadInput.h"
#include "device_defines.h"


///////////////////////////////////////////////////////////////////////////////////////////////
//

class KeypadInput : public IKeypadInput
{
public:
    KeypadInput() {}

    virtual int Read(uint8_t pin) {
        return pin == BTN_SEL ?  1 : 0;
    }
};


/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput()
{
    static KeypadInput _input;

    return &_input;
}