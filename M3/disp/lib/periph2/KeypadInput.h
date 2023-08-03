// KeypadInput.h
//

#ifndef __KEYPAD_ABSTRACTION_H__
#define __KEYPAD_ABSTRACTION_H__

/////////////////////////////////////////////////////////////////////////////////////
//

struct IKeypadInput
{
    virtual void PreRead() = 0;
    virtual int Read(uint8_t pin) = 0;
};



/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput();


#endif // __KEYPAD_ABSTRACTION_H__
