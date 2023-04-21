// HostCommand.cpp
//

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "HostCommand.h"
#include "logger.h"


///////////////////////////////////////////////////////////////////////////
// class HostCommand

HostCommand::HostCommand()
{
    reset();
}

void HostCommand::reset()
{
    memset(mBuffer, 0, sizeof(mBuffer));
    mPos = 0;
}

int HostCommand::push(int ch)
{
    int cmd = CMD_UNKNOWN;

    if (ch == '\r' || ch == '\n')
    {
        if (mPos > 0)
        {
            LOGv(">> %s", mBuffer);
            if (strncmp(mBuffer, "MUTE ", 5) == 0)
            {
                cmd = CMD_MUTE;
                
                mLastCommand.cmd = CMD_MUTE;
                mLastCommand.mute.state = parseBoolean(&mBuffer[5]) ? 1 : 0; 
            }
        }

        reset();
    }
    else
    {
        if (mPos < sizeof(mBuffer) - 1)
            mBuffer[mPos++] = ch;
    }

    return cmd;
}

bool HostCommand::parseBoolean(const char* str)
{
    // True, False, On, Off, (number): mute(> 0), unmute(== 0)
    if (strcasecmp(str, "true") == 0)
        return true;
    if (strcasecmp(str, "false") == 0)
        return false;
    if (strcasecmp(str, "on") == 0)
        return true;
    if (strcasecmp(str, "off") == 0)
        return false;
    if (strcasecmp(str, "T") == 0)
        return true;
    if (strcasecmp(str, "F") == 0)
        return false;

    return atoi(str) == 0 ? false : true;
}