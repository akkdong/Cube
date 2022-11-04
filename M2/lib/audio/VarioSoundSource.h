// VarioSoundSource.h
//

#ifndef __VARIO_SOUND_SOURCE_H__
#define __VARIO_SOUND_SOURCE_H__

#include <Arduino.h>



///////////////////////////////////////////////////////////////////////////////////
//

class VarioSoundSource
{
public:
    VarioSoundSource();

public:
    bool begin(uint32_t sampleRate = 44100);

    void setFrequency(uint16_t freq);
    /*
    void setSampleRate(uint32_t rate) 
    {
        m_sampleRate = rate;
        m_deltaTime = 1.0 / rate;
        m_cycles = 0;
    }
    uint32_t getSampleRate() { return m_sampleRate; }
    */

    virtual uint32_t read(void* data, uint32_t len);

protected:
    int16_t readSample();

protected:
    volatile uint32_t  m_frequency;
    uint32_t m_sampleRate;

    float m_amplitude;
    float m_cycles; // varies between 0.0 and 1.0
    float m_deltaTime;
};


#endif // __VARIO_SOUND_SOURCE_H__
