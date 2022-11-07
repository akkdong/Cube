// tca9554.cpp
//

#include "tca9554.h"


/////////////////////////////////////////////////////////////////////////////////////
//

TCA9554::TCA9554(int address, TwoWire& wire)
    : _address(address), _wire(wire)
{
}

void TCA9554::writeRegister(uint8_t reg, uint8_t value)
{
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.write(value);
  _wire.endTransmission();
}

uint8_t TCA9554::readRegister(uint8_t reg)
{
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.endTransmission();
  _wire.requestFrom(_address, 1u);

  return _wire.read();
}


void TCA9554::setConfig(uint8_t value)
{
    writeRegister(CONFIG_REG, value);
}

void TCA9554::setOutput(uint8_t value)
{
    writeRegister(OUTPUT_PORT_REG, value);
}

void TCA9554::setPolarity(uint8_t value)
{
    writeRegister(POLARITY_INVERSION_REG, value);
}



///////////////////////////////////////////////////////////////////////////
//

TCA9554 exio;
