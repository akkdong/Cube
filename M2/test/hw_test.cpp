// hw_test.cpp
//

#include <Arduino.h>

#include "board.h"
#include "adc_driver.h"
#include "es8311.h"
#include "tca9554.h"
#include "bme280.h"

#include "AudioGeneratorAAC.h"
#include "AudioFileSourcePROGMEM.h"
#include "sampleaac.h"

#include "AudioOutputI2SEx.h"


AudioFileSourcePROGMEM *in;
AudioGeneratorAAC *aac;
AudioOutputI2SEx *out;

TCA9554       io;
ES8311        codec;


void setup()
{
    Serial.begin(115200);
    Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
    Serial.println("HW Test...");

    io.setOutput(0b10110000);
    io.setConfig(0b00001111);    

    adc_init();

    codec.codec_config(AUDIO_HAL_44K_SAMPLES);
    codec.codec_set_voice_volume(80);
    uint16_t id;
    uint8_t version;
    codec.read_chipid(id, version);
    Serial.printf("ID: %04X, version: %02X\r\n", id, version);

    audioLogger = &Serial;
    in = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));
    aac = new AudioGeneratorAAC();
    out = new AudioOutputI2SEx();
    out->SetPinout(GPIO_I2S_MCLK, GPIO_I2S_SCLK, GPIO_I2S_LRCK, GPIO_I2S_DOUT);
    out->SetOutputModeMono(true); // <-- must use mono-mode

    aac->begin(in, out);
}

void loop()
{
#if TEST_ADC
    float voltage = adc_get_voltage();
    Serial.printf("BAT: %f\r\n", voltage);

    delay(1000);
#else
    if (aac->isRunning()) {
        aac->loop();
    } else {
        Serial.println("AAC done");
        delay(1000);
    }   
#endif
}
