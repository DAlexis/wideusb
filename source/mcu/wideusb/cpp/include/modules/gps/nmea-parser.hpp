#ifndef NMEA_PARSER_HPP_INCLUDED
#define NMEA_PARSER_HPP_INCLUDED

#include "modules/gps/minmea.h"
#include <string>

struct GPSData
{
    minmea_sentence_rmc rmc;
    minmea_sentence_gga gga;
    minmea_sentence_gst gst;
    minmea_sentence_gsv gsv;
    minmea_sentence_vtg vtg;
    minmea_sentence_zda zda;
    minmea_sentence_gll gll;

    void parse(const std::string& str);
    void parse_line(const char* line);
};

class NMEAParser
{
public:
    void parse(const std::string& str);
    const GPSData& current_result();

private:
    GPSData m_current_data;
};

#endif // NMEA_PARSER_HPP_INCLUDED
