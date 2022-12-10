// LocationParser.h
//

#ifndef __LOCATION_PARSER_H__
#define __LOCATION_PARSER_H__

#include <stdint.h>
#include <time.h>

#include "TaskBase.h"
#include "CriticalSection.h"

#include "abstract/LocationDataSource.h"

#define MAX_NMEA_PARSER_BUFFER					(512)

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


/////////////////////////////////////////////////////////////////////////////
// class DataQueue

class DataQueue
{
public:
	DataQueue();

public:
	void				push(int ch);
	int					pop();

	int					front() { return mFront; }

	int					isFull() 	{ return ((mFront + 1) % MAX_NMEA_PARSER_BUFFER) == mTail; }
	bool				isEmpty()	{ return mHead == mTail; }	

	int					get(int index);
	int					copy(char* dst, int startPos, int count);

	void				acceptReserve();
	void				rejectReserve();


	// debugging stubs
	void				dumpReserve();


protected:
	char				mBuffer[MAX_NMEA_PARSER_BUFFER];
	volatile int		mHead;
	volatile int		mTail;
	volatile int		mFront;
};



////////////////////////////////////////////////////////////////////////////////////////
//

class LocationParser : public TaskBase, public CriticalSection
{
public:
    LocationParser();

public:
    void                begin(ILocationDataSource* iLocation, bool useTask = true);
    void                end();
    void                reset();

    void                update();

    bool                isFixed() { return mFixed; }

    //
	bool		        availableNmea();
	int			        readNmea(); 

	int					availableIGC();
	int					readIGC();   

    bool                availableLocation() { return mDataReady; }
    void                resetLocation() { mDataReady = false; }

    time_t              getDateTime() { return mDateTime; }
    float               getLatitude() { return mLatitude; }
    float               getLongitude() { return mLongitude; }
    float               getAltitude() { return mAltitude; }
    int16_t             getSpeed() { return mSpeed; }
    int16_t             getHeading() { return mHeading; }

    //
    void                setTimeZone(float tz) { mTimeZone = tz; }

protected:
	void				TaskProc() override;

private:
	int 				timeStr2TmStruct(struct tm * _tm, int startPos);
	int	 				dateStr2TmStruct(struct tm * _tm, int startPos);
	
	void				parseField(int fieldIndex, int startPos);
	
	float				strToFloat(int startPos, int limit = -1);
	long				strToNum(int startPos, int limit = -1);
	
	long				floatToCoordi(float value);
	
private:
	//
    ILocationDataSource* mDataSourcePtr;
	DataQueue			mDataQueue;
	volatile int		mFieldStart;
	volatile int		mFieldIndex;
	
	//
	volatile int		mParseStep; 		// -1 or 0 ~ 12
	volatile uint8_t	mParseState;		// 
	volatile uint8_t	mParity;
	
	//
	volatile bool		mDataReady;

	//uint32_t			mDate;
	//uint32_t			mTime;
	
	struct tm 			mTmStruct;
	time_t				mTimeCurr; // time of latest parsing sentence
	time_t				mTimeLast; // time of last parsing sentence
	time_t				mDateTime; // current date & time
    float               mTimeZone;

	float				mLatitude;
	float				mLongitude;
	float				mAltitude;
	int16_t				mSpeed;
	int16_t				mHeading;

	volatile bool		mFixed;

	// IGC sentence
	char				mIGCSentence[MAX_IGC_SENTENCE+1];
	volatile int		mIGCNext;	// next = 0 ~ MAX_XXX -1 -> available
	volatile int		mIGCSize;	// size = 0 -> empty, size = MAX_xx -> valid
};


#endif // __LOCATION_PARSER_H__
