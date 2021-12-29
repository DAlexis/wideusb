#include "wideusb/utils/gps/nmea-parser.hpp"
#include "wideusb/utils/utilities.hpp"

#include <string.h>
#include <chrono>

using namespace std::literals::chrono_literals;

GPSData::GPSData()
{
    zerify(m_rmc);
    zerify(m_gga);
    zerify(m_gsa);
    zerify(m_gst);
    zerify(m_gsv);
    zerify(m_vtg);
    zerify(m_zda);
    zerify(m_gll);
}

bool GPSData::parse_line(const char* line, std::chrono::steady_clock::time_point time)
{
    bool result = true;
    switch(minmea_sentence_id(line, false))
    {
    case MINMEA_SENTENCE_RMC:
        result = minmea_parse_rmc(&m_rmc, line);
        if (!result)
            break;
        update_lat_lon(m_rmc.latitude, m_rmc.longitude);
        minmea_gettime(&m_time, &m_rmc.date, &m_rmc.time);
        m_os_ticks_last_time_update = time;
        break;

    case MINMEA_SENTENCE_GGA:
    {
        result = minmea_parse_gga(&m_gga, line);
        if (!result)
            break;
        update_lat_lon(m_gga.latitude, m_gga.longitude);
        timespec time_candidate;
        minmea_gettime(&time_candidate, &m_rmc.date, &m_gga.time);
        update_time_if_newer(time_candidate);
        break;
    }

    case MINMEA_SENTENCE_GSA:
        result = minmea_parse_gsa(&m_gsa, line);
        break;

    case MINMEA_SENTENCE_GLL:
    {
        result = minmea_parse_gll(&m_gll, line);
        if (!result)
            break;

        update_lat_lon(m_gll.latitude, m_gll.longitude);
        timespec time_candidate;
        minmea_gettime(&time_candidate, &m_rmc.date, &m_gll.time);
        update_time_if_newer(time_candidate);
        break;
    }

    case MINMEA_SENTENCE_GST:
        result = minmea_parse_gst(&m_gst, line);
        break;

    case MINMEA_SENTENCE_GSV:
        result = minmea_parse_gsv(&m_gsv, line);
        break;

    case MINMEA_SENTENCE_VTG:
        result = minmea_parse_vtg(&m_vtg, line);
        break;

    case MINMEA_SENTENCE_ZDA:
        result = minmea_parse_zda(&m_zda, line);
        break;

    case MINMEA_UNKNOWN:
        result = false;
        break;

    default:
    case MINMEA_INVALID:
        result = false;
        break;
    }
    /*if (!result)
        return result;*/
    return result;
}

Point GPSData::point() const
{
    Point result;
    result.time = m_time;
    result.latitude = m_latitude;
    result.longitude = m_longitude;
    result.altitude = minmea_tofloat(&m_gga.altitude);

    result.last_update_ticks = std::max(m_os_ticks_last_pps, m_os_ticks_last_time_update);
    return result;
}

void GPSData::fit_to_pps( std::chrono::steady_clock::time_point ticks_from_last_pps)
{
    if (ticks_from_last_pps - m_os_ticks_last_time_update > 500ms)
    {
        // It seems that we have next pps, but last time update over NMEA was too long ago
        m_time.tv_sec++;
        m_os_ticks_last_time_update = ticks_from_last_pps;
    }
    m_os_ticks_last_pps = ticks_from_last_pps;

}

void GPSData::update_time_if_newer(const timespec& time)
{
    if (time.tv_sec > m_time.tv_sec || (time.tv_sec == m_time.tv_sec && time.tv_nsec > m_time.tv_nsec) )
    {
        m_time = time;
    }
}

void GPSData::update_lat_lon(const minmea_float& lat, const minmea_float& lon)
{
    m_latitude = minmea_tocoord(&lat);
    m_longitude = minmea_tocoord(&lon);
}

