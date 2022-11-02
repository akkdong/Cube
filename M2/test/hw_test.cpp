// hw_test.cpp
//

#include <Arduino.h>

#include "board.h"
#include "adc_driver.h"
#include "es8311.h"
#include "tca9554.h"
#include "bme280.h"

#include "AudioFileSourceFunction.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorAAC.h"

#include "AudioOutputMixer.h"
#include "AudioOutputI2SEx.h"

#include "sampleaac.h"
#include "viola.h"

#define TEST_ADC        1
#define TEST_AAC        2
#define TEST_WAV        3

#define TEST_METHOD     TEST_AAC

#if TEST_METHOD != TEST_ADC
#if TEST_METHOD == TEST_AAC
AudioFileSourcePROGMEM * audioSrc;
AudioGeneratorAAC * audioGen;
#elif TEST_METHOD == TEST_WAV
AudioFileSourcePROGMEM* audioSrc[2];
AudioGeneratorWAV* audioGen[2];
AudioOutputMixer *mixer;
AudioOutputMixerStub *stub[2];
float hz = 440.f;
#endif
AudioOutputI2SEx* audioOut;
#endif

TCA9554       io;
ES8311        codec;


void setup()
{
    Serial.begin(115200);
    USBSerial.begin();
    Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
    Serial.println("HW Test...");
    USBSerial.println("HW Test...");

    io.setOutput(0b10110000);
    io.setConfig(0b00001111);    

    adc_init();

    codec.codec_config(AUDIO_HAL_32K_SAMPLES);
    codec.codec_set_voice_volume(80);
    uint16_t id;
    uint8_t version;
    codec.read_chipid(id, version);
    USBSerial.printf("ID: %04X, version: %02X\r\n", id, version);

    audioLogger = &Serial;

    #if TEST_METHOD == TEST_ADC
    // nop
    #elif TEST_METHOD == TEST_AAC
    audioSrc = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));

    audioOut = new AudioOutputI2SEx();
    audioOut->SetPinout(GPIO_I2S_MCLK, GPIO_I2S_SCLK, GPIO_I2S_LRCK, GPIO_I2S_DOUT);
    audioOut->SetOutputModeMono(true); // <-- must use mono-mode

    audioGen = new AudioGeneratorAAC();
    audioGen->begin(audioSrc, audioOut);
    #elif TEST_METHOD == TEST_WAV
    audioSrc[0] = new AudioFileSourcePROGMEM( viola, sizeof(viola) );

    audioOut = new AudioOutputI2SEx();
    audioOut->SetPinout(GPIO_I2S_MCLK, GPIO_I2S_SCLK, GPIO_I2S_LRCK, GPIO_I2S_DOUT);
    audioOut->SetOutputModeMono(true); // <-- must use mono-mode

    mixer = new AudioOutputMixer(32, audioOut);
    stub[0] = mixer->NewInput();
    stub[0]->SetGain(0.3);

    audioGen[0] = new AudioGeneratorWAV();
    audioGen[0]->begin(audioSrc[0], audioOut);    
    #endif
}

void loop()
{
#if TEST_METHOD == TEST_ADC
    float voltage = adc_get_voltage();
    USBSerial.printf("BAT: %f\r\n", voltage);

    delay(1000);
#elif TEST_METHOD == TEST_AAC
    if (audioGen->isRunning()) 
    {
        if (!audioGen->loop())
            audioGen->stop();
    } 
    else 
    {
        USBSerial.println("AAC done");
        delay(1000);
    }       
#elif TEST_METHOD == TEST_WAV
    static uint32_t start = 0;
    static bool go = false;

    if (!start) start = millis();

    if (audioGen[0]->isRunning()) {
        if (!audioGen[0]->loop()) { 
            audioGen[0]->stop(); 
            stub[0]->stop(); 
            USBSerial.printf("stopping 1\r\n"); 
        }
    }

    if (millis()-start > 3000) {
        if (!go) {
            USBSerial.printf("starting 2\r\n");
            stub[1] = mixer->NewInput();
            stub[1]->SetGain(0.4);
            audioSrc[1] = new AudioFileSourcePROGMEM(viola, sizeof(viola));
            audioGen[1] = new AudioGeneratorWAV();
            audioGen[1]->begin(audioSrc[1], stub[1]);
            go = true;
        }
        if (audioGen[1]->isRunning()) {
            if (!audioGen[1]->loop()) { 
                audioGen[1]->stop(); 
                stub[1]->stop(); 
                USBSerial.printf("stopping 2\r\n");
            }
        }
    }
     
#endif
}
