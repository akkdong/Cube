// CubeSentence.h
//

#ifndef __CUBE_SENTENCE_H__
#define __CUBE_SENTENCE_H__

#define MAX_CUBE_SENTENCE_BUFFER    (2048)



/////////////////////////////////////////////////////////////////////////////////////////
// class CubeSentence

class CubeSentence
{
public:
    CubeSentence();

    int             available();
    int             read();

    int             start(const char* id);
    int             append(float value, int precision);
    int             append(int value);
    int             finish();

protected:
    int             write(char ch);

    char            NibbleToHexa(unsigned char ch);

protected:
    char            mBuffer[MAX_CUBE_SENTENCE_BUFFER];
    unsigned int    mHead;
    unsigned int    mTail;
    unsigned int    mFront;

    char            mCheckSum;
};


#endif // __CUBE_SENTENCE_H__
