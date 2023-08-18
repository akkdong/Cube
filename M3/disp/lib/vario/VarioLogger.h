// VarioLogger.h
//

#ifndef __VARIO_LOGGER_H__
#define __VARIO_LOGGER_H__

#include <Arduino.h>
#include <time.h>

#include "FixedLenDigit.h"
#include "FS.h"


/////////////////////////////////////////////////////////////////////////////
// class VarioLogger

class VarioLogger
{
public:
	VarioLogger();
	
public:
	bool				begin(time_t date); // date = seconds since 1900
	void				end(time_t date);
	
	size_t				write(uint8_t ch);
	size_t				write(void* buf, size_t bufLen);

	void				updateBaroAltitude(float varioAlt);
	
	int					isInitialized();
	int					isLogging();

private:
	void				reset();
	
	const char *		makeFileName(char * buf, time_t date);
	void				writeHeader(time_t date);

	int					validateName(int ch);

private:
	File				root;
	File				file;
	
	uint8_t				logState;
	int					columnCount;
	
	float				varioAltitude;
	FixedLenDigit		digit;

    //
    char                pilot[32];
    char                glider_manufacture[32];
    char                glider_model[32];
};


#endif // __VARIO_LOGGER_H__
