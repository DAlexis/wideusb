#include "modules/gps/nmea-parser.hpp"
#include "utilities.hpp"

#include <string.h>

GPSData::GPSData(uint32_t ticks_per_sec) :
    m_ticks_per_sec(ticks_per_sec)
{
    zerify(rmc);
    zerify(gga);
    zerify(gsa);
    zerify(gst);
    zerify(gsv);
    zerify(vtg);
    zerify(zda);
    zerify(gll);
}

bool GPSData::parse_line(const char* line, size_t ticks)
{
    bool result = true;
    switch(minmea_sentence_id(line, false))
    {
    case MINMEA_SENTENCE_RMC:
        result = minmea_parse_rmc(&rmc, line);
        if (!result)
            break;
        update_lat_lon(rmc.latitude, rmc.longitude);
        minmea_gettime(&m_time, &rmc.date, &rmc.time);
        m_ticks_last_time_update = ticks;
        break;

    case MINMEA_SENTENCE_GGA:
    {
        result = minmea_parse_gga(&gga, line);
        if (!result)
            break;
        update_lat_lon(gga.latitude, gga.longitude);
        timespec time_candidate;
        minmea_gettime(&time_candidate, &rmc.date, &gga.time);
        update_time_if_newer(time_candidate);
        break;
    }

    case MINMEA_SENTENCE_GSA:
        result = minmea_parse_gsa(&gsa, line);
        break;

    case MINMEA_SENTENCE_GLL:
    {
        result = minmea_parse_gll(&gll, line);
        if (!result)
            break;

        update_lat_lon(gll.latitude, gll.longitude);
        timespec time_candidate;
        minmea_gettime(&time_candidate, &rmc.date, &gll.time);
        update_time_if_newer(time_candidate);
        break;
    }

    case MINMEA_SENTENCE_GST:
        result = minmea_parse_gst(&gst, line);
        break;

    case MINMEA_SENTENCE_GSV:
        result = minmea_parse_gsv(&gsv, line);
        break;

    case MINMEA_SENTENCE_VTG:
        result = minmea_parse_vtg(&vtg, line);
        break;

    case MINMEA_SENTENCE_ZDA:
        result = minmea_parse_zda(&zda, line);
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
    result.altitude = minmea_tofloat(&gga.altitude);

    result.last_update_ticks = std::max(m_ticks_last_pps, m_ticks_last_time_update);
    return result;
}

void GPSData::fit_to_pps(size_t ticks)
{
    if (ticks - m_ticks_last_time_update > m_ticks_per_sec / 2)
    {
        // It seems that we have next pps, but last time update over NMEA was too long ago
        m_time.tv_sec++;
        m_ticks_last_time_update = ticks;
    }
    m_ticks_last_pps = ticks;

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

