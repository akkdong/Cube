// IGCSentence.h
//

#ifndef __IGC_SENTENCE_H__
#define __IGC_SENTENCE_H__

#define MAX_IGC_SENTENCE						(37)	// B-sentence max size
														// ex: B1602405407121N00249342WA0028000421rn
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



//////////////////////////////////////////////////////////////////////////////
// class IGCSentence

class IGCSentence
{
public:
    IGCSentence();

public:
    void            begin(long stime, float lat, float lon, float altB, float altG);
    
	int				available();	
	int				read();

    void            dump(Stream& stm);

protected:
    float           n2d(float nmea);
    int             d2n(float decimal);

protected:
    char            mSentence[MAX_IGC_SENTENCE+1];
    int             mPos;
};

#endif // __IGC_SENTENCE_H__