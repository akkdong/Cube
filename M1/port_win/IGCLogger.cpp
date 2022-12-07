// IGCLogger.cpp
//

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "IGCLogger.h"


// IGC parsing settings
//

#define MAX_IGC_SENTENCE						(37)	// B-sentence max size
														// ex: B1602405407121N00249342WA0028000421\r\n
														//     0123456789012345678901234567890123456789
														//     BHHMMSSDDMMmmmNDDDMMmmmWAPPPPPGGGGGCL
														//                   S        EV          RF

#define IGC_OFFSET_START						(0)
#define IGC_OFFSET_TIME							(1)
#define IGC_OFFSET_LATITUDE						(7)
#define IGC_OFFSET_LATITUDE_					(14)
#define IGC_OFFSET_LONGITUDE					(15)
#define IGC_OFFSET_LONGITUDE_					(23)
#define IGC_OFFSET_VALIDITY						(24)
#define IGC_OFFSET_PRESS_ALT					(25)
#define IGC_OFFSET_GPS_ALT						(30)
#define IGC_OFFSET_RETURN						(35)
#define IGC_OFFSET_NEWLINE						(36)
#define IGC_OFFSET_TERMINATE					(37)

#define IGC_SIZE_TIME							(6)
#define IGC_SIZE_LATITUDE						(7)
#define IGC_SIZE_LATITUDE_						(1)
#define IGC_SIZE_LONGITUDE						(8)
#define IGC_SIZE_LONGITUDE_						(1)
#define IGC_SIZE_VALIDITY						(1)
#define IGC_SIZE_PRESS_ALT						(5)
#define IGC_SIZE_GPS_ALT						(5)


// 
#define CLEAR_STATE()		logState = 0
#define SET_STATE(bit)		logState |= (bit)
#define UNSET_STATE(bit)	logState &= ~(bit)

#define IS_SET(bit)			(logState & (bit)) == (bit)


#define LOGGER_INIT_FAILED		(1 << 0)
#define LOGGER_WORKING			(1 << 1)


//
const char * nameManufacture = "MIN";
const char * serialNumber = "XXX";
const char * tailOfFileName = "01.igc";

const char * logsFolder = "/TrackLogs";

char pathBuf[64];


// ANRCSTM Variometer & GPS Loggger
// HFDTExxxx
// HFPLTPILOT:xxxxxx
// HFCCLCOMPETITION CLASS:FAI-3 (PG)
// HFGTYGLIDERTYPE:xxxxx
// HFDTM100GPSDATUM:WGS-84
const char * igcHeader[] =
{
	"AXXX CUBE Variometer & GPS Loggger v1", 
	"\r\nHFDTE",					// ex) DDMMYY
	NULL,
	"\r\nHFPLTPILOT:",				// ex) akkdong
	NULL,
	"\r\nHFCCLCOMPETITION CLASS:",	// ex) FAI-3 (PG)
	NULL,
	"\r\nHFGTYGLIDERTYPE:", 		// ex) NIVIUK IcePeak6  {MANUFACTURE} {MODEL}
	NULL,
	"\r\nHFDTM100GPSDATUM:WGS-84",
	NULL,
	"\r\n"
};

#define IGC_HEADER_DATE			(2)
#define IGC_HEADER_PILOT		(4)
#define IGC_HEADER_CLASS		(6)
#define IGC_HEADER_GLIDER		(8)
#define IGC_HEADER_GPSDATUM		(10)

/////////////////////////////////////////////////////////////////////////////
//

#define FILE_WRITE      "w"

class File
{
public:
    File() {
        memset(this->buf, 0, sizeof(this->buf));
    }
    File(const File& f) {
        memcpy(this->buf, f.buf, sizeof(this->buf));
    }

    size_t printf(const char* fmt, ...) { 
        va_list args;        
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);

        return print(buf); 
    }

    size_t print(const char* str) { 
        size_t n = 0;
        while (*str)
            n += write(*str++);

        return n; 
    }

    size_t write(const uint8_t* data, size_t len) { 
        size_t n = 0;
        while(len > 0) {
            n += write(*data++);
            --len;
        }

        return n;
    }

    size_t write(uint8_t ch) { 
        fputc(ch, stdout);
        return 1; 
    }

    void close() { }

    operator bool() { return true; }

private:
    char buf[128];

} file;

class SdCard
{
public:
    File open(const char* file, const char* mode) { 
        printf("SD_CARD.open(%s, %s)\n", file, mode);

        return File(); 
    }

    bool valid() { 
        return true; 
    }

    bool exists(const char* path) {
        printf("SD_CARD.exits(%s)\n", path);
        return false; 
    }

    bool mkdir(const char* path) { 
        printf("SD_CARD.mkdir(%s)\n", path);
        return true; 
    }

} SD_CARD;

/////////////////////////////////////////////////////////////////////////////
// class IGCLogger

IGCLogger::IGCLogger()
{
	reset();

    strcpy(pilot, "DUMMY");
    strcpy(glider_manufacture, "Notorious");
    strcpy(glider_model, "Rascal");
}

bool IGCLogger::begin(time_t date)
{
	if (! SD_CARD.valid())
	{
		SET_STATE(LOGGER_INIT_FAILED);
		return false;
	}

	// 
	if (! SD_CARD.exists(logsFolder))
	{
		if (! SD_CARD.mkdir(logsFolder))
		{
			SET_STATE(LOGGER_INIT_FAILED);
			return false;
		}
	}	
	
	// create new file // YYYY-MM-DD-NRC-STM-nn.igc
	char name[26];
	
	if (! makeFileName(name, date))
	{
		// no valid file room
		SET_STATE(LOGGER_INIT_FAILED);
		return false;
	}

	strcpy(pathBuf, logsFolder);
	strcat(pathBuf, "/");
	strcat(pathBuf, name);

	if ((file = SD_CARD.open(pathBuf, FILE_WRITE)))
	{
		//
		SET_STATE(LOGGER_WORKING);
		
		// write header
		writeHeader(date);
		
		return true;
	}
	
	return false;
}

void IGCLogger::end(time_t date)
{
	if (! IS_SET(LOGGER_WORKING))
		return;
	
	// if (date)
	// {
	// 	time_t localdate = date + (timezone * 60 * 60); 
	// 	struct tm * _tm = localtime(&localdate);	
	//	
	// 	file.timestamp(T_ACCESS|T_WRITE,
	// 					_tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday,
	// 					_tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	// }
	
	UNSET_STATE(LOGGER_WORKING);
	file.close();
}

int IGCLogger::write(uint8_t ch)
{
	if (! IS_SET(LOGGER_WORKING))
		return 0;
	
	#if 1	
	if (columnCount < 0 && ch != 'B')
		return 0;
	
	if (ch == 'B')
		columnCount = 0;
	
	// pressure altitude field is replaced by measured pressure altitude : NmeaParser returns null value
	if (columnCount == IGC_OFFSET_PRESS_ALT)
		digit.begin(varioAltitude, IGC_SIZE_PRESS_ALT);
	
	if (digit.available())
		ch = digit.read();
	
	file.write((const uint8_t *)&ch, 1);
	columnCount += 1;
	
	if (columnCount == MAX_IGC_SENTENCE)
		columnCount = -1; // eol
	#else
	file.write((const uint8_t *)&ch, 1);
	#endif
	
	return 1;
}

void IGCLogger::updateBaroAltitude(float varioAlt)
{
	varioAltitude = varioAlt;
	//Serial.println(varioAltitude);
}

int IGCLogger::isInitialized()
{
	return IS_SET(LOGGER_INIT_FAILED) ? false : true;
}

int IGCLogger::isLogging()
{
	return IS_SET(LOGGER_WORKING) ? true : false;
}

void IGCLogger::reset()
{
	logState		= 0;
	columnCount 	= -1;
	varioAltitude 	= 0.0;
}

int IGCLogger::validateName(int ch)
{
	if ('A' <= ch && ch <= 'Z')
		return ch;
	if ('a' <= ch && ch <= 'z')
		return (ch - 'a') + 'A';
	if ('0' <= ch && ch <= '9')
		return ch;

	return '_';
}

const char * IGCLogger::makeFileName(char * buf, time_t date)
{
	// name format : YYYY-MM-DD-NRC-XXX-nn.igc
	// ...
	int pos = 0;
	int i, num;
	const char * ptr;
	struct tm * _tm;

	_tm = localtime(&date);
	
	// year
	digit.begin(_tm->tm_year + 1900, 4);
	while (digit.available())
		buf[pos++] = digit.read();
	buf[pos++] = '-';
	// month
	digit.begin(_tm->tm_mon + 1, 2);
	while (digit.available())
		buf[pos++] = digit.read();
	buf[pos++] = '-';
	// day
	digit.begin(_tm->tm_mday, 2);
	while (digit.available())
		buf[pos++] = digit.read();
	buf[pos++] = '-';

	// manufacture code
	for (i = 0, ptr = nameManufacture; i < 3; i++)
		buf[pos++] = (*ptr) ? *ptr++ : 'X';
	buf[pos++] = '-';
		
	// FR serial number
	ptr = pilot[0] ? pilot : serialNumber;
	for (i = 0; i < 3; i++)
		buf[pos++] = validateName(ptr[i]); // (*ptr) && (*ptr != 0x20) ? *ptr++ : 'X';
	buf[pos++] = '-';
	
	// flight number of the day & file extension
	for (ptr = tailOfFileName; *ptr; ptr++)
		buf[pos++] = *ptr;
	buf[pos] = '\0'; // null-terminate
	
	for (i = 2; i < 101; i++) // valid number : 01 ~ 99
	{
		strcpy(pathBuf, logsFolder);
		strcat(pathBuf, "/");
		strcat(pathBuf, buf);

		if (! SD_CARD.exists(pathBuf))
			return buf;
		
		buf[19] = (i / 10) + '0';
		buf[20] = (i % 10) + '0';
	}
	
	return NULL;
}

void IGCLogger::writeHeader(time_t date)
{
	if(! IS_SET(LOGGER_WORKING))
		return;

	//
	for (int i = 0; i < sizeof(igcHeader) / sizeof(igcHeader[0]); i++)
	{
		switch (i)
		{
		case IGC_HEADER_DATE	 :
			{
				struct tm * _tm;
				_tm = gmtime(&date);				
				
				//  DD : tm_mday -> 1 ~ 31
				digit.begin(_tm->tm_mday, 2);
				while (digit.available())
					file.write(digit.read());
				// MM : tm_mon -> 0 ~ 11
				digit.begin(_tm->tm_mon + 1, 2);
				while (digit.available())
					file.write(digit.read());
				// YY : tm_year -> xxxx
				digit.begin(_tm->tm_year % 100, 2);
				while (digit.available())
					file.write(digit.read());
				
				//
//				time_t localdate = date + (timezone * 60 * 60); 
//				_tm = localtime(&localdate);	
//				
//				file.timestamp(T_ACCESS|T_CREATE|T_WRITE,
//								_tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday,
//								_tm->tm_hour, _tm->tm_min, _tm->tm_sec);
			}
			break;
		case IGC_HEADER_PILOT	 :
			if (pilot[0])
				file.print(pilot);
			break;
		case IGC_HEADER_CLASS	 :
			break;
		case IGC_HEADER_GLIDER	 :
			if (glider_manufacture[0] || glider_model[0])
				file.printf("%s %s", glider_manufacture, glider_model);
			break;
		case IGC_HEADER_GPSDATUM :
			// leave it empty!!
			break;
		default 				 :
			file.print(igcHeader[i]);
			break;
		}
	}
}
