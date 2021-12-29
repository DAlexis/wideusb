#include "wideusb/utils/gps/nmea-parser.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>


const char sample[] =
    "$GPRMC,142500.000,A,5432.1990,N,01234.4244,E,0.35,321.98,041100,,,A\n"
    "$GPVTG,321.98,T,,M,0.35,N,0.64,K,A\n"
    "$GPGGA,142501.000,5432.1985,N,01234.4249,E,1,4,3.18,131.5,M,9.3,M,,\n"
    "$GPGLL,5432.1985,N,01234.4249,E,142501.000,A,A\n"
    "$GPGSA,A,3,32,20,21,18,,,,,,,,,3.33,3.18,0.98\n"
    "$GPGSV,4,1,13,10,82,165,,27,58,226,,21,54,120,31,20,54,078,24\n"
    "$GPGSV,4,2,13,08,49,286,,11,23,299,,32,22,170,20,18,18,109,25\n"
    "$GPGSV,4,3,13,15,15,041,,24,10,081,,14,05,179,,16,04,226,\n"
    "$GPGSV,4,4,13,28,02,358,\n";

void add_checksum(std::string& nmea_line)
{
    int c = 0;
    for (size_t i = 1; i < nmea_line.size(); i++)
    {
        c ^= nmea_line[i];
    }
    std::ostringstream oss;
    oss << nmea_line << "*" << std::hex << c;
    nmea_line = oss.str();
}

TEST(GPS, NmeaParseTypical)
{
    using namespace std::literals::chrono_literals;
    GPSData gps;

    bool success = true;
    std::istringstream f(sample);
    std::string line;

    auto time_point = std::chrono::steady_clock::now();

    while (std::getline(f, line)) {
        add_checksum(line);
        success = success && gps.parse_line(line.c_str(), time_point);
//        std::cout << line << std::endl;
    }

    Point p = gps.point();
    ASSERT_NEAR(p.latitude, 54.53665 , 1e-5);
    ASSERT_NEAR(p.longitude, 12.573748 , 1e-5);
    tm t = *localtime(&p.time.tv_sec);
    ASSERT_EQ(t.tm_hour, 14);
    ASSERT_EQ(t.tm_min, 25);
    ASSERT_EQ(t.tm_sec, 01);
    ASSERT_EQ(p.last_update_ticks, time_point);

    time_point += 100ms;
    gps.fit_to_pps(time_point);
    p = gps.point();
    ASSERT_EQ(p.last_update_ticks, time_point);

    time_point += 800ms;
    // Assume we have next pps, but time was not updated by GPS yet
    gps.fit_to_pps(time_point);
    p = gps.point();
    ASSERT_EQ(p.last_update_ticks, time_point);
    t = *localtime(&p.time.tv_sec);
    ASSERT_EQ(t.tm_sec, 02);
}
