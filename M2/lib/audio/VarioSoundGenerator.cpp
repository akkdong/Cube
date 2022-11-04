// VarioSoundGenerator.cpp
//

#include "VarioSoundGenerator.h"


///////////////////////////////////////////////////////////////////////////////////
// VarioSoundGenerator implements

VarioSoundGenerator::VarioSoundGenerator() 
    : TaskBase("Vario", 4 * 1024, 1)
    , mSource()
    , mOutputPtr(nullptr) 
    , mPlayMode(PLAY_BEEP)
    , mPlayFreq(0.0f)
{
}

bool VarioSoundGenerator::begin(AudioOutput* output, uint32_t sampleRate)
{
    //
    mPlayFreq = 0;

    mSource.setFrequency(0);
    mSource.begin(sampleRate);

    //
    mOutputPtr = output;
    mOutputPtr->SetRate(sampleRate);
    mOutputPtr->SetBitsPerSample(16);
    mOutputPtr->SetChannels(1);
    mOutputPtr->begin();

    return create();
}

void VarioSoundGenerator::stop()
{
    mOutputPtr->stop();
    this->destroy();
}

void VarioSoundGenerator::play(int freq)
{
    mPlayFreq = freq;
}

void VarioSoundGenerator::TaskProc() 
{
    float lastFreq = 0;
    int16_t lastSample = 0;

    while (1)
    {
        // playmode: EFFECT
        //
        // ....


        // playmode: BEEP
        int16_t samples[2] = { 0, 0 };
        mSource.read(&samples[0], 2);

        // case 1: freq 0 -> x
        // case 2: freq x -> y
        // case 3: freq x -> 0
        if (lastFreq != mPlayFreq)
        {
            // case 1: setFreq, change lastFreq, read sample
            // case 2: setFreq, change lastFreq, read sample
            // case 3: setFreq, change lastFreq, reset sample <--- if does zero-crossing
            if (mPlayFreq > 0 || lastSample * samples[0] > 0)
            {
                mSource.setFrequency(mPlayFreq);
                lastFreq = mPlayFreq;

                mSource.read(&samples[0], 2);
            }
        }

        lastSample = samples[1] = samples[0];
        mOutputPtr->ConsumeSample(samples);
    }
}
