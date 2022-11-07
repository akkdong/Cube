// hw_test.cpp
//

#include <Arduino.h>

#include "board.h"
#include "logger.h"
#include "adc_driver.h"
#include "es8311.h"
#include "tca9554.h"
#include "bme280.h"
#include "TaskBase.h"
#include "keypad.h"

#include "AudioFileSourceFunction.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorAAC.h"

#include "AudioOutputMixer.h"
#include "AudioOutputI2SEx.h"

#include "sampleaac.h"
#include "viola.h"

#include "SD.h"

#define TEST_ADC        1
#define TEST_AAC        2
#define TEST_WAV        3
#define TEST_SD         4
#define TEST_KEYPAD     5

#define TEST_METHOD     TEST_KEYPAD

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

ES8311        codec;


#if TEST_METHOD == TEST_SD
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    USBSerial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        USBSerial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        USBSerial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            USBSerial.print("  DIR : ");
            USBSerial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            USBSerial.print("  FILE: ");
            USBSerial.print(file.name());
            USBSerial.print("  SIZE: ");
            USBSerial.println(file.size());
        }
        file = root.openNextFile();
    }
}

#include "driver/spi_common.h"
#include "vfs_api.h"
#include "sd_diskio.h"

class MySD : public SDFS
{
public:
    MySD() : SDFS(FSImplPtr(new VFSImpl())), _spi(SPI2_HOST) {

    }
    bool begin() {
        _spi.begin(GPIO_SPI_SCLK, GPIO_SPI_MISO, GPIO_SPI_MOSI, GPIO_SPI_CS_SD);

        _pdrv = sdcard_init(GPIO_SPI_CS_SD, &_spi, 8000000);
        if(_pdrv == 0xFF) {
            return false;
        }

        if(!sdcard_mount(_pdrv, "/sd", 5, false)){
            sdcard_unmount(_pdrv);
            sdcard_uninit(_pdrv);
            _pdrv = 0xFF;
            return false;
        }

        _impl->mountpoint("/sd");
        return true;
    }

    SPIClass _spi;
};

MySD    sd;

#endif


#if TEST_METHOD == TEST_KEYPAD

#include "../src/vario_porting/KeypadInput.cpp"

struct MyKeypadCallback : public KeypadCallback
{
    virtual void OnPressed(uint8_t key) {
        USBSerial.printf("OnPressed: %x\r\n", key);
    }
    virtual void OnLongPressed(uint8_t key) {
        USBSerial.printf("OnLongPressed: %x\r\n", key);
    }
    virtual void OnReleased(uint8_t key) {
        USBSerial.printf("OnReleased: %x\r\n", key);
    }
} _callback;

Keypad keypad(&_callback);

#endif

void setup()
{
    USBSerial.begin(115200);
    Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL, (uint32_t)400000);
    USBSerial.println("HW Test...");

    exio.setOutput(0b00100000); // 0b10110000 : TOUCH, BOOST, AUDIO, LCD, IO3, IO2, IO1, IO0
    exio.setConfig(0b00001111);    

    #if TEST_METHOD == TEST_AAC || TEST_METHOD == TEST_WAV
    codec.codec_config(AUDIO_HAL_32K_SAMPLES);
    codec.codec_set_voice_volume(80);
    uint16_t id;
    uint8_t version;
    codec.read_chipid(id, version);
    USBSerial.printf("ID: %04X, version: %02X\r\n", id, version);
    #endif

    #if TEST_METHOD == TEST_ADC
    adc_init();
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
    #elif TEST_METHOD == TEST_SD
    if (!sd.begin())
    {
        USBSerial.println("Card mount failed!");
        return;
    }
    uint8_t cardType = sd.cardType();

    if(cardType == CARD_NONE){
        USBSerial.println("No SD card attached");
        return;
    }

    USBSerial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        USBSerial.println("MMC");
    } else if(cardType == CARD_SD){
        USBSerial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        USBSerial.println("SDHC");
    } else {
        USBSerial.println("UNKNOWN");
    } 

    uint64_t cardSize = sd.cardSize() / (1024 * 1024);
    USBSerial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(sd, "/Music", 0);
    #elif TEST_METHOD == TEST_KEYPAD
    keypad.begin(CreateKeypadInput());
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
#elif TEST_METHOD == TEST_KEYPAD
    #if RAW_TEST
    uint8_t input = exio.getInput();
    USBSerial.printf("INPUT: %X, %d\r\n", input, digitalRead(GPIO_KEY));
    delay(1000);
    #else
    keypad.update();
    delay(10);
    #endif
#endif
}
