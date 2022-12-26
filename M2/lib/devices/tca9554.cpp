// tca9554.cpp
//

#include "tca9554.h"


/////////////////////////////////////////////////////////////////////////////////////
//

TCA9554::TCA9554(int address, TwoWireEx& wire)
    : _address(address), _wire(wire)
{
}

void TCA9554::writeRegister(uint8_t reg, uint8_t value)
{
  _wire.lock();
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.write(value);
  _wire.endTransmission();
  _wire.unlock();
}

uint8_t TCA9554::readRegister(uint8_t reg)
{
  _wire.lock();
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.endTransmission(false);
  _wire.requestFrom(_address, 1u);

  uint8_t ret = _wire.read();
  _wire.unlock();

  return ret;
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
