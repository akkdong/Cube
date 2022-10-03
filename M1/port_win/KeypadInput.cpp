// KeypadInput.cpp
//

#include <stdint.h>

#include "abstract/KeypadInput.h"


///////////////////////////////////////////////////////////////////////////////////////////////
//

class KeypadInput : public IKeypadInput
{
public:
    KeypadInput() {}

    virtual int Read(uint8_t pin) {
        return 0;
    }
};


/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput()
{
    static KeypadInput _input;

    return &_input;
}