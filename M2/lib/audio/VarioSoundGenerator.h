// VarioSoundGenerator.h
//

#ifndef __VARIO_SOUND_GENERATOR_H__
#define __VARIO_SOUND_GENERATOR_H__

#include <Arduino.h>

#include "VarioSoundSource.h"
#include "AudioOutput.h"
#include "TaskBase.h"


///////////////////////////////////////////////////////////////////////////////////
// class VarioSoundGenerator

class VarioSoundGenerator : public TaskBase
{
public:
    VarioSoundGenerator();

    enum PlayMode {
        PLAY_BEEP,
        PLAY_EFFECT
    };


public:
    bool begin(AudioOutput* output, uint32_t sampleRate = 44100);
    void stop();

    void play(int freq);

protected:
	void TaskProc() override;

protected:
    VarioSoundSource    mSource;
    AudioOutput*        mOutputPtr;

    volatile PlayMode   mPlayMode;
    volatile int        mPlayFreq;
};



#endif // __VARIO_SOUND_GENERATOR_H__
