// audio_test.cpp
//

#include <Arduino.h>

#include "board.h"
#include "es8311.h"
#include "tca9554.h"

#include "AudioFileSourceFunction.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SEx.h"
#include "TaskBase.h"
#include "TaskWatchdog.h"


class VarioSoundSource : public AudioFileSource
{
public:
    VarioSoundSource()
        : m_frequency(0.0f)
        , m_amplitude(32767.0)
        , m_cycles(0.0f)
        , m_deltaTime(0.0f)
        , m_phase(0.0f)
        , double_PI(PI * 2.0f)
        , m_sampleRate(44100)
    {
    }

public:
    bool begin(uint32_t sampleRate = 44100) 
    {
        m_sampleRate = sampleRate;
        m_deltaTime = 1.0 / sampleRate;

        return true;
    }

    void setFrequency(uint16_t freq) 
    {
        m_frequency = freq;
        m_cycles = 0;
    }

    /*
    void setSampleRate(uint32_t rate) 
    {
        m_sampleRate = rate;
        m_deltaTime = 1.0 / rate;
        m_cycles = 0;
    }

    uint32_t getSampleRate() { return m_sampleRate; }
    */

    uint32_t read(void* data, uint32_t len) override {
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

protected:
    int16_t readSample() {
        float angle = double_PI * m_cycles + m_phase;
        int16_t result = m_amplitude * sinf(angle);

        m_cycles += m_frequency * m_deltaTime;
        if (m_cycles > 1.0)
            m_cycles -= 1.0;

        return result;
    }

protected:
    volatile float  m_frequency;

    float m_amplitude = 1.0;
    float m_cycles = 0.0; // varies between 0.0 and 1.0
    float m_deltaTime = 0.0;
    float m_phase = 0.0;
    float double_PI = PI * 2.0;

    uint32_t m_sampleRate = 44100;
};



class VarioSoundGenerator : public TaskBase
{
public:
    VarioSoundGenerator() 
        : TaskBase("Vario", 1024, 1)
        , mSource()
        , mOutputPtr(nullptr) 
        , mPlayMode(PLAY_BEEP)
        , mPlayFreq(0.0f)
        , mLastSample{ 0, 0 }
    {
    }


    enum PlayMode {
        PLAY_BEEP,
        PLAY_FILE
    };


public:
    bool begin(AudioOutput* output, uint32_t sampleRate = 44100) 
    {
        mSource.setFrequency(0);
        mSource.begin(sampleRate);

        mOutputPtr = output;
        mOutputPtr->SetRate(sampleRate);
        mOutputPtr->SetBitsPerSample(16);
        mOutputPtr->SetChannels(1);
        mOutputPtr->begin();

        mLastSample[0] = 0;
        mLastSample[1] = 0;
        mOutputPtr->ConsumeSample(mLastSample);

        mPlayFreq = 0;

        return create();
    }

    void stop()
    {
        mOutputPtr->stop();
        this->destroy();
    }

    void play(int freq)
    {
        USBSerial.printf("play: %d\r\n", freq);
        mPlayFreq = freq;
    }

protected:
	void TaskProc() 
    {
        float activeFreq = 0;

        while (1)
        {
            bool updated = (activeFreq != mPlayFreq || mPlayFreq > 0) ? true : false;

            if (activeFreq != mPlayFreq)
            {
                mSource.setFrequency(mPlayFreq);
                activeFreq = mPlayFreq;
            }

            if (updated)
            {
                if (mPlayFreq > 0)
                {
                    int16_t sample;
                    mSource.read(&sample, 2);

                    mLastSample[AudioOutput::LEFTCHANNEL] = sample;
                    mLastSample[AudioOutput::RIGHTCHANNEL] = sample;
                }
                else
                {
                    mLastSample[AudioOutput::LEFTCHANNEL] = 0;
                    mLastSample[AudioOutput::RIGHTCHANNEL] = 0;
                }

                mOutputPtr->ConsumeSample(mLastSample);
            }
        }
    }

protected:
    VarioSoundSource    mSource;
    AudioOutput*        mOutputPtr;

    volatile PlayMode   mPlayMode;
    volatile int        mPlayFreq;

    int16_t             mLastSample[2];
};




TCA9554       io;
ES8311        codec;

VarioSoundGenerator audioGen;
AudioOutputI2SEx audioOut;


void setup()
{
    USBSerial.begin();
    Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
    USBSerial.println("Audio Test...");

    io.setOutput(0b10110000); // 0b10110000 : Touch, Boost, Audio, Lcd, 3,2,1,0
    io.setConfig(0b00001111);
    delay(10);

    codec.codec_config(AUDIO_HAL_16K_SAMPLES);
    codec.codec_set_voice_volume(72);
    uint16_t id;
    uint8_t version;
    codec.read_chipid(id, version);
    USBSerial.printf("Codec ID: %04X, version: %02X\r\n", id, version);    

    //
	TaskWatchdog::begin(1000);
	TaskWatchdog::add(NULL);    

    audioOut.SetPinout(GPIO_I2S_MCLK, GPIO_I2S_SCLK, GPIO_I2S_LRCK, GPIO_I2S_DOUT);
    audioOut.SetOutputModeMono(true);

    audioGen.begin(&audioOut, 16000);
}

void loop()
{
    static uint32_t lastTick = millis();
    static int freq = 440;
    static int play = 0;

    //
	TaskWatchdog::reset();    

    //
    while (millis() - lastTick > 1000)
    {
        if (play == 0)
        {
            audioGen.play(0);
        }
        else
        {
            audioGen.play(freq);

            freq += 20;
            if (freq > 600)
                freq = 440;
        }

        play = 1 - play;
        lastTick = millis();
    }
}
