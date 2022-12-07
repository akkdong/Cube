// IGCLogger.h
//

#ifndef __IGC_LOGGER_H__
#define __IGC_LOGGER_H__

#include <stdint.h>
#include <time.h>

#include "FixedLenDigit.h"


/////////////////////////////////////////////////////////////////////////////
// class IGCLogger

class IGCLogger
{
public:
    IGCLogger();

public:
	bool				begin(time_t date); // date = seconds since 1900
	void				end(time_t date);
	
	int					write(uint8_t ch);

	void				updateBaroAltitude(float varioAlt);
	
	int					isInitialized();
	int					isLogging();

private:
	void				reset();
	
	const char *		makeFileName(char * buf, time_t date);
	void				writeHeader(time_t date);

	int					validateName(int ch);

private:
//  File				root;
//  File				file;
	
	uint8_t				logState;
	int					columnCount;
	
	float				varioAltitude;
	FixedLenDigit		digit;

    //
    char                pilot[32];
    char                glider_manufacture[32];
    char                glider_model[32];
};

#endif // __IGC_LOGGER_H__
