// HostCommand.h
//

#ifndef __HOST_COMMAND_H__
#define __HOST_COMMAND_H__

#include <stdint.h>

#define MAX_BUFFERSIZE      (64)


///////////////////////////////////////////////////////////////////////////
// class HostCommand

class HostCommand
{
public:
    HostCommand();

    enum CommandType {
        CMD_UNKNOWN,
        CMD_MUTE,
    };

    struct Mute {
        uint8_t state; // 0: unmute, others: mute
    };

    struct Command {
        CommandType cmd;
        union {
            Mute mute;
        };
    };

public:
    void reset();
    int push(int ch);

    const Command& getLastCommand() {
        return mLastCommand;
    }

protected:
    bool parseBoolean(const char* str);
    
protected:
    char mBuffer[MAX_BUFFERSIZE];
    unsigned int mPos;

    Command mLastCommand;
};

#endif // __HOST_COMMAND_H__
