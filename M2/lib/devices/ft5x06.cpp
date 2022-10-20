// ft5x06.cpp
//

#include "board.h"
#include "ft5x06.h"


/////////////////////////////////////////////////////////////////////////////////
//

FT5x06::FT5x06(uint8_t address, TwoWire& wire) : _address(address), _wire(wire)
{
}

void FT5x06::begin()
{
    // Valid touching detect threshold
    writeByte(FT5X06_ID_G_THGROUP, 70);

    // valid touching peak detect threshold
    writeByte(FT5X06_ID_G_THPEAK, 60);

    // Touch focus threshold
    writeByte(FT5X06_ID_G_THCAL, 16);

    // threshold when there is surface water
    writeByte(FT5X06_ID_G_THWATER, 60);

    // threshold of temperature compensation
    writeByte(FT5X06_ID_G_THTEMP, 10);

    // Touch difference threshold
    writeByte(FT5X06_ID_G_THDIFF, 20);

    // Delay to enter 'Monitor' status (s)
    writeByte(FT5X06_ID_G_TIME_ENTER_MONITOR, 2);

    // Period of 'Active' status (ms)
    writeByte(FT5X06_ID_G_PERIODACTIVE, 12);

    // Timer to enter 'idle' when in 'Monitor' (ms)
    writeByte(FT5X06_ID_G_PERIODMONITOR, 40);
}



uint8_t FT5x06::readByte(uint8_t reg)
{
    _wire.beginTransmission(_address);
    _wire.write(reg);
    _wire.endTransmission(false);

    _wire.requestFrom(_address, (uint8_t)1);
    return _wire.read();
}

void FT5x06::readBytes(uint8_t reg, size_t len, uint8_t* data)
{
    _wire.beginTransmission(_address);
    _wire.write(reg);
    _wire.endTransmission(false);

    _wire.requestFrom(_address, len);
    for (size_t i = 0; i < len; i++)
        data[i] = _wire.read();
}

void FT5x06::writeByte(uint8_t reg, uint8_t data)
{
    _wire.beginTransmission(_address);
    _wire.write(reg);
    _wire.endTransmission();
}

uint8_t FT5x06::getTouchPointsNum()
{
    return readByte(FT5X06_TD_STATUS);
}

bool FT5x06::getPosition(uint16_t* x, uint16_t* y)
{
    if (getTouchPointsNum() == 0)
        return false;

    uint8_t data[4];
    readBytes(FT5X06_TOUCH1_XH, 4, data);

#if LCD_DIRECTION_LANDSCAPE
    *y = LCD_HEIGHT - ((data[0] & 0x0f) << 8) - data[1];
    *x = ((data[2] & 0x0f) << 8) + data[3];
#else
    *x = ((data[0] & 0x0f) << 8) + data[1];
    *y = ((data[2] & 0x0f) << 8) + data[3];
#endif

    return true;
}
