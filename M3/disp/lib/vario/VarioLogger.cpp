// VarioLogger.cpp
//

#include "VarioLogger.h"
#include "SD.h"
#include "logger.h"


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
#define CLEAR_STATE()							logState = 0
#define SET_STATE(bit)							logState |= (bit)
#define UNSET_STATE(bit)						logState &= ~(bit)

#define IS_SET(bit)								(logState & (bit)) == (bit)


#define LOGGER_INIT_FAILED						(1 << 0)
#define LOGGER_WORKING							(1 << 1)


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
	"AXXX M3 Variometer & GPS Loggger v2", 
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
// class VarioLogger

VarioLogger::VarioLogger()
{
	reset();
}

bool VarioLogger::begin(time_t date)
{
	LOGv("Start IGC Logging... : %u", date);

	// 
	reset();

	// 
	if (SD.cardType() == CARD_NONE)
	{
		SET_STATE(LOGGER_INIT_FAILED);
		LOGv(" --> No SD Card!!");
		return false;
	}

	if (! SD.exists(logsFolder))
	{
		if (! SD.mkdir(logsFolder))
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

	if ((file = SD.open(pathBuf, FILE_WRITE)))
	{
		LOGi("Open IGC File : %s", pathBuf);

		//
		SET_STATE(LOGGER_WORKING);
		
		// write header
		writeHeader(date);
		
		return true;
	}
	
	return false;
}

void VarioLogger::end(time_t date)
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

size_t VarioLogger::write(uint8_t ch)
{
	if (! IS_SET(LOGGER_WORKING))
		return 0;

	#if 0
	#if 0	
	if (columnCount < 0 && ch != 'B')
		return 0;
	
	if (ch == 'B')
		columnCount = 0;
	
	// pressure altitude field is replaced by measured pressure altitude : NmeaParser returns null value
	if (columnCount == IGC_OFFSET_PRESS_ALT)
		digit.begin(varioAltitude + 0.5, IGC_SIZE_PRESS_ALT);
	
	if (digit.available())
		ch = digit.read();
	
	file.write((const uint8_t *)&ch, 1);
	columnCount += 1;
	
	if (columnCount == MAX_IGC_SENTENCE)
		columnCount = -1; // eol
	#else
	file.write((const uint8_t *)&ch, 1);
	#endif
	#else
	int next = (front + 1) % (sizeof(buffer) / sizeof(buffer[0]));
	if (next != rear)
	{
		buffer[front] = ch;
		front = next;
		return 1;
	}
	#endif
	
	return 0;
}

size_t VarioLogger::write(void* buf, size_t bufLen)
{
	#if 0
	return file.write((const uint8_t *)buf, bufLen);
	#else
	uint8_t *_buf = (uint8_t *)buf;
	size_t i;
	for (i = 0; i < bufLen; i++)
	{
		int next = (front + 1) % (sizeof(buffer) / sizeof(buffer[0]));
		if (next == rear)
			break;

		buffer[front] = _buf[i];
		front = next;
	}
	return i;
	#endif
}

int VarioLogger::flush()
{
	if (! IS_SET(LOGGER_WORKING))
		return 0;

	int count = 0;
	while (front != rear)
	{
		file.write(&buffer[rear], 1);
		rear = (rear + 1) % (sizeof(buffer) / sizeof(buffer[0]));
		count += 1;
	}

	return count;
}

void VarioLogger::setPilotInfo(const char* name, const char* g_model, const char* g_manu)
{
	//memset(pilot, 0, sizeof(pilot));
	//memset(glider_manufacture, 0, sizeof(glider_manufacture));
	//memset(glider_model, 0, sizeof(glider_model));

	strncpy(pilot, name, sizeof(pilot) - 1);
	strncpy(glider_model, g_model, sizeof(glider_model) - 1);
	strncpy(glider_manufacture, g_manu, sizeof(glider_manufacture) - 1);
}

//void VarioLogger::updateBaroAltitude(float varioAlt)
//{
//	varioAltitude = varioAlt;
//	//Serial.println(varioAltitude);
//}

int VarioLogger::isInitialized()
{
	return IS_SET(LOGGER_INIT_FAILED) ? false : true;
}

int VarioLogger::isLogging()
{
	return IS_SET(LOGGER_WORKING) ? true : false;
}

void VarioLogger::reset()
{
	logState		= 0;
	columnCount 	= -1;
//	varioAltitude 	= 0.0;

	memset(pilot, 0, sizeof(pilot));
	memset(glider_manufacture, 0, sizeof(glider_manufacture));
	memset(glider_model, 0, sizeof(glider_model));

	// empty buffer
	front = rear = 0;
}

int VarioLogger::validateName(int ch)
{
	if ('A' <= ch && ch <= 'Z')
		return ch;
	if ('a' <= ch && ch <= 'z')
		return (ch - 'a') + 'A';
	if ('0' <= ch && ch <= '9')
		return ch;

	return '_';
}

const char * VarioLogger::makeFileName(char * buf, time_t date)
{
	// name format : YYYY-MM-DD-NRC-XXX-nn.igc
	// ...
	FixedLenDigit digit;
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

		if (! SD.exists(pathBuf))
			return buf;
		
		buf[19] = (i / 10) + '0';
		buf[20] = (i % 10) + '0';
	}
	
	return NULL;
}

void VarioLogger::writeHeader(time_t date)
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
				FixedLenDigit digit;
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
