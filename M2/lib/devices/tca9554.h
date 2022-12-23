/*
  TCA9554.h - Library for work with TCA9554 or TCA9554a GPIO expander
  Created by Prohorov Vladimir, Octomber 14, 2019.
  Released into the public domain.
*/
#ifndef TCA9554_H
#define TCA9554_H

#include <Arduino.h>
#include "TwoWireEx.h"


#define TCA9554A_ADDR               (0x3C)

#define INPUT_PORT_REG              (0x00)
#define OUTPUT_PORT_REG             (0x01)
#define POLARITY_INVERSION_REG      (0x02)
#define CONFIG_REG                  (0x03)


/////////////////////////////////////////////////////////////////////////////////////
//

class TCA9554
{
public:
    TCA9554(int address = TCA9554A_ADDR, TwoWireEx& wire = WireEx);

public:
    void setConfig(uint8_t value);
    void setOutput(uint8_t value);
    void setPolarity(uint8_t value);

    uint8_t getInput()          { return readRegister(INPUT_PORT_REG); }
    uint8_t getOutput()         { return readRegister(OUTPUT_PORT_REG); }
    uint8_t getPolarity()       { return readRegister(POLARITY_INVERSION_REG); }
    uint8_t getConfig()         { return readRegister(CONFIG_REG); }

protected:
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);

private:
    int         _address;
    TwoWireEx&  _wire;
};


///////////////////////////////////////////////////////////////////////////
//

class EXIO : public TCA9554
{
public:
    EXIO() {}

    enum PIN {
        EXT_IO0,
        EXT_IO1,
        EXT_IO2,
        EXT_IO3,
        EXT_LCD_BL,
        EXT_AUDIO_PA,
        EXT_BOOST_EN,
        EXT_TOUCH_RST,
    };

    void update();

    int get(PIN pin);

protected:
    uint8_t     DIR;
    uint8_t     CFG;
    uint8_t     DAT;
};

/////////////////////////////////////////////////////////////////////////////////////
//

extern TCA9554 exio;

#endif
