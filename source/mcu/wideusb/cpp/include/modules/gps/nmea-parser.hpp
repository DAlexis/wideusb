#ifndef NMEA_PARSER_HPP_INCLUDED
#define NMEA_PARSER_HPP_INCLUDED


#include "modules/gps/point.hpp"
#include "modules/gps/minmea.h"
#include <string>
#include <time.h>

class GPSData
{
public:
    GPSData(uint32_t ticks_per_sec = 1000);
    bool parse_line(const char* line, size_t ticks);
    Point point() const;
    void fit_to_pps(size_t ticks);

private:
    minmea_sentence_rmc rmc;
    minmea_sentence_gga gga;
    minmea_sentence_gsa gsa;
    minmea_sentence_gst gst;
    minmea_sentence_gsv gsv;
    minmea_sentence_vtg vtg;
    minmea_sentence_zda zda;
    minmea_sentence_gll gll;

    struct timespec m_time;
    size_t m_ticks_last_time_update = 0;
    size_t m_ticks_last_pps = 0;
    uint32_t m_ticks_per_sec;

    float m_latitude = 0.0f, m_longitude = 0.0f;

    void update_time_if_newer(const timespec& time);
    void update_lat_lon(const minmea_float& lat, const minmea_float& lon);
};

#endif // NMEA_PARSER_HPP_INCLUDED
