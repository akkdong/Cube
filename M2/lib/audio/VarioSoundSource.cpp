// VarioSoundSource.cpp
//

#include "VarioSoundSource.h"




///////////////////////////////////////////////////////////////////////////////////
//

VarioSoundSource::VarioSoundSource()
    : m_frequency(0)
    , m_sampleRate(44100)
    , m_amplitude(32767.0)
    , m_cycles(0.0f)
    , m_deltaTime(0.0f)
{
}

bool VarioSoundSource::begin(uint32_t sampleRate) 
{
    m_sampleRate = sampleRate;
    m_deltaTime = 1.0 / sampleRate;

    return true;
}

void VarioSoundSource::setFrequency(uint16_t freq) 
{
    m_frequency = freq;
    //m_cycles = 0;
}


uint32_t VarioSoundSource::read(void* data, uint32_t len) 
{
    if ((len % 2) != 0)
        return 0;
    
    uint8_t* ptr = reinterpret_cast<uint8_t *>(data);
    uint32_t size = 0;
    while (size < len)
    {
        int16_t sample = readSample();
        uint8_t* samplePtr = reinterpret_cast<uint8_t *>(&sample);
        
        *ptr++ = samplePtr[0];
        size++;
        *ptr++ = samplePtr[1];
        size++;
    }

    return size;
}

int16_t VarioSoundSource::readSample() 
{
    if (m_frequency == 0)
        return 0;

    float angle = PI * 2.0F * m_cycles;
    int16_t result = m_amplitude * sinf(angle);

    m_cycles += m_frequency * m_deltaTime;
    if (m_cycles > 1.0)
        m_cycles -= 1.0;

    return result;
}
