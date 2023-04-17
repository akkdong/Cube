// NmeaParser.h
//

#ifndef __NMEA_PARSER_H__
#define __NMEA_PARSER_H__

#include <stdint.h>
#include <time.h>


//////////////////////////////////////////////////////////////////////////////
// class NmeaParser

class NmeaParser
{
public:
    NmeaParser();

    struct Satellite {
        uint8_t num;
        uint8_t elevation;
        uint16_t azimuth;
        uint8_t snr;
    };

    enum Statement {
        STAT_UNKNOWN = 0,
        STAT_GGA,
        STAT_GSA,
        STAT_GSV,
        STAT_RMC,
        STAT_VAR,
        STAT_KBD,
        STAT_INVALID_CHKSUM = UINT8_MAX
    };

    struct GGA {
        time_t time;        
        float latitude;
        float longitude;
        uint8_t fixQuality;
        uint8_t statellites;
        float altitude;
    };

    struct RMC {
        time_t time;
        time_t date;
        uint8_t status;
        float latitude;
        float longitude;
        float speed;
        float heading;
    };

    struct VAR {
        float pressure;
        float temperature;
        float vspeed;
    };

    struct KBD {
        uint8_t key;
        uint8_t state;
    };


public:
    int begin();
    int update(int ch);

protected:
    void parseField();
    bool checkCRC();

    time_t strToTime(const char* str);
    time_t strToDate(const char* str, time_t time);

    float nmeaToDecimal(float nmea);
	long floatToCoordi(float value);

    uint8_t hexaToNumber(char ch);
    uint8_t hexaToNumber(const char* str);


protected:
    //
    uint8_t fixed; // 0: Invalid, 1: GPS Fix, 2: DGPS Fix, 3: PPS Fix
    uint8_t fixMode; // 1: NO Fix, 2: 2D Fix, 3: 3D Fix
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float cource; // heading

    time_t time;
    time_t date;

    uint8_t satellitesIDs[12];
    Satellite satelittes[12];
    
    // parser context
    struct ParserContext {
        Statement statement;
        uint8_t checksum;
        uint8_t validateChecksum;
        char field[15];
        uint8_t fieldPos;
        uint8_t fieldNum;

        union {
            GGA gga;
            RMC rmc;
            VAR var;
            KBD kbd;
        };
    };

    ParserContext parserContext;
};

#endif // __NMEA_PARSER_H__
