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
    KeypadInput() : inputData(0) {}

    void PreRead() override {
        inputData = exio.getInput();
    }

    int Read(uint8_t pin) override {
        if (pin & GPIO_EXT_MASK)
        {
            #if 0
            uint8_t input = exio.getInput();
            return (input & (1 << (pin & (~GPIO_EXT_MASK)))) ? 1 : 0;
            #else
            return (inputData & (1 << (pin & (~GPIO_EXT_MASK)))) ? 1 : 0;
            #endif
        }
        
        return digitalRead(pin);
    }

protected:
    uint8_t inputData;
};


/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput()
{
    static KeypadInput _input;

    return &_input;
}