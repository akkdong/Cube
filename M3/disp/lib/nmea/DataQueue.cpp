// DataQueue.cpp
//

#include "DataQueue.h"
#include "logger.h"


/////////////////////////////////////////////////////////////////////////////
// class DataQueue

DataQueue::DataQueue()
{
	mHead = mTail = mFront = 0;
}

void DataQueue::push(int ch)
{
	mBuffer[mFront] = ch;
	mFront = (mFront + 1) % MAX_NMEA_PARSER_BUFFER;
}

int DataQueue::pop()
{
	if (mHead == mTail)
		return -1;

	int ch = mBuffer[mTail];
	mTail = (mTail + 1) % MAX_NMEA_PARSER_BUFFER;

	return ch;
}

int DataQueue::get(int index)
{
	return mBuffer[index % MAX_NMEA_PARSER_BUFFER];
}

int DataQueue::copy(char* dst, int startPos, int count)
{
	for (int i = 0; i < count; i++)
		dst[i] = mBuffer[(startPos + i) % MAX_NMEA_PARSER_BUFFER];

	return count;
}

void DataQueue::acceptReserve()
{
	mHead = mFront;
}

void DataQueue::rejectReserve()
{
	mFront = mHead;
}

void DataQueue::dumpReserve()
{
	for(int i = mHead; i != mFront; )
	{
		int ch = mBuffer[i];
		if (ch == '\r' || ch == '\n')
			ch = '.';
		trace_putc(ch);

		i = (i + 1) % MAX_NMEA_PARSER_BUFFER;
	}
}
