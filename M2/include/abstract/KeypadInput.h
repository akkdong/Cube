// KeypadInput.h
//

#ifndef __KEYPAD_ABSTRACTION_H__
#define __KEYPAD_ABSTRACTION_H__

/////////////////////////////////////////////////////////////////////////////////////
//

#define KEY_RETURN          (0xB0)
#define KEY_ESC             (0xB1)
#define KEY_LEFT_ARROW      (0xD8)
#define KEY_RIGHT_ARROW     (0xD7)
#define KEY_UP_ARROW        (0xDA)
#define KEY_DOWN_ARROW      (0xD9)


struct IKeypadInput
{
    virtual int Read(uint8_t pin) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

IKeypadInput* CreateKeypadInput();


#endif // __KEYPAD_ABSTRACTION_H__
