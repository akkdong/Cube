// CubeSentence.cpp
//

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "CubeSentence.h"
#include "Digit.h"


/////////////////////////////////////////////////////////////////////////////////////////
// class CubeSentence


CubeSentence::CubeSentence()
{
}



int CubeSentence::available()
{
    return (mHead - mTail) & (MAX_CUBE_SENTENCE_BUFFER - 1);
}

int CubeSentence::read()
{
    if (mHead == mTail)
        return -1;

    int ch = mBuffer[mTail];
    mTail = (mTail + 1) & (MAX_CUBE_SENTENCE_BUFFER - 1);

    return ch;
}


int CubeSentence::start(const char* id)
{
    if (write('$') == 0)
        return 0;
    mCheckSum = 0;

    for (const char* ptr = id; *ptr; ptr++)
    {
        if (write(*ptr) == 0)
            return 0;

        mCheckSum ^= *ptr;
    }

    return (mFront - mHead) & (MAX_CUBE_SENTENCE_BUFFER - 1);
}

int CubeSentence::append(float value, int precision)
{
    int len = 0;

    Digit digit;
    digit.begin(value, precision);
    
    write(',');
    while (digit.available())
    {
        char ch = digit.get();
        write(ch);

        mCheckSum ^= ch;
        len += 1;
    }

    return len;
}

int CubeSentence::append(int value)
{
    int len = 0;
    
    Digit digit;
    digit.begin((long)value);

    write(',');
    while (digit.available())
    {
        int ch = digit.get();
        write(ch);

        mCheckSum ^= ch;
        len += 1;
    }

    return len;
}

int CubeSentence::finish()
{    
    write('*');
    write(NibbleToHexa((unsigned char)mCheckSum >> 4));
    write(NibbleToHexa((unsigned char)mCheckSum & 0x0F));
    write('\r');
    write('\n');

    int n = (mFront - mHead) & (MAX_CUBE_SENTENCE_BUFFER - 1);
    mHead = mFront;

    return n;
}


int CubeSentence::write(char ch)
{
    unsigned int next = (mFront + 1) & (MAX_CUBE_SENTENCE_BUFFER - 1);
    if (next == mTail)
        return 0; // full

    mBuffer[mFront] = ch;
    mFront = next;

    return 1;
}

char CubeSentence::NibbleToHexa(unsigned char ch)
{
    if (ch < 10)
        return '0' + ch;

    if (ch < 16)
        return 'A' + (ch - 10);

    return 0;
}
