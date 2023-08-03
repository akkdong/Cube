// SdCard.cpp
//

#include "SdCard.h"

#include "driver/spi_common.h"
#include "vfs_api.h"
#include "sd_diskio.h"
#include "board.h"


/////////////////////////////////////////////////////////////////////////////
// class SDInternal

class SDInternal : public SDFS
{
public:
    SDInternal() : SDFS(FSImplPtr(new VFSImpl())), _spi(SPI2_HOST) {}

public:
    bool begin() {
        _spi.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS_SD);

        _pdrv = sdcard_init(SPI_CS_SD, &_spi, 8000000);
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

protected:
    SPIClass _spi;
};

static SDInternal  sd;



/////////////////////////////////////////////////////////////////////////////
// class SdCard

SdCard::SdCard() : mState(-1)
{
}

int SdCard::begin()
{
    if (mState < 0)
    {
        mState = 1;

        if (! sd.begin() || sd.cardType() == CARD_NONE)
            mState = 0;
    }

    return mState;
}

File SdCard::open(const char* path, const char* mode)
{
    if (valid())
        return sd.open(path, mode);

    return File();
}

bool SdCard::exists(const char* path)
{
    if (valid())
        return sd.exists(path);

    return false;
}

bool SdCard::remove(const char* path)
{
    if (valid())
        return sd.remove(path);

    return false;
}

bool SdCard::rename(const char* pathFrom, const char* pathTo)
{
    if (valid())
        return sd.rename(pathFrom, pathTo);

    return false;
}

bool SdCard::mkdir(const char *path)
{
    if (valid())
        return sd.mkdir(path);

    return false;
}

bool SdCard::rmdir(const char *path)
{
    if (valid())
        return sd.rmdir(path);

    return false;
}


/////////////////////////////////////////////////////////////////////////////
//

SdCard  SD_CARD;