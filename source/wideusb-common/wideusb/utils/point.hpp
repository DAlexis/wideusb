#ifndef POINT_HPP_INCLUDED
#define POINT_HPP_INCLUDED

#include <ctime>
#include <chrono>

struct Point
{
    Point();

    float latitude = 0.0f;
    float longitude = 0.0f;
    float altitude = 0.0f;

    timespec time;

    std::chrono::steady_clock::time_point last_update_ticks;
    bool has_pps = false;
};

#endif // POINT_HPP_INCLUDED
