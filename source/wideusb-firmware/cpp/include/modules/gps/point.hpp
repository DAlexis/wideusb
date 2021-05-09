#ifndef POINT_HPP_INCLUDED
#define POINT_HPP_INCLUDED

#include <ctime>

struct Point
{
    Point();

    float latitude = 0.0f;
    float longitude = 0.0f;
    float altitude = 0.0f;

    timespec time;

    size_t last_update_ticks = 0;
    bool has_pps = false;
};

#endif // POINT_HPP_INCLUDED
