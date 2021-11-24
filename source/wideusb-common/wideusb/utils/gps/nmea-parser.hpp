#ifndef NMEA_PARSER_HPP_INCLUDED
#define NMEA_PARSER_HPP_INCLUDED

#include "wideusb/utils/point.hpp"
#include "wideusb/utils/gps/minmea.h"
#include <string>
#include <ctime>

class GPSData
{
public:
    GPSData(uint32_t ticks_per_sec = 1000);
    bool parse_line(const char* line, size_t os_ticks);
    Point point() const;

    /**
     * @brief Update round part of seconds when PPS received, if it was not already done by NMEA line
     * @param current PPS timer ticks
     */
    void fit_to_pps(size_t ticks_from_last_pps);

private:
    minmea_sentence_rmc m_rmc;
    minmea_sentence_gga m_gga;
    minmea_sentence_gsa m_gsa;
    minmea_sentence_gst m_gst;
    minmea_sentence_gsv m_gsv;
    minmea_sentence_vtg m_vtg;
    minmea_sentence_zda m_zda;
    minmea_sentence_gll m_gll;

    struct timespec m_time;
    size_t m_os_ticks_last_time_update = 0;
    size_t m_os_ticks_last_pps = 0;
    uint32_t m_ticks_per_sec;

    float m_latitude = 0.0f, m_longitude = 0.0f;

    void update_time_if_newer(const timespec& time);
    void update_lat_lon(const minmea_float& lat, const minmea_float& lon);
};

#endif // NMEA_PARSER_HPP_INCLUDED
