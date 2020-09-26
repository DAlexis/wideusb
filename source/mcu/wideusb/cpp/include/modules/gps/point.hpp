#ifndef POINT_HPP_INCLUDED
#define POINT_HPP_INCLUDED

#include <time.h>

struct Point
{
    Point();
    struct tm get_tm() const;

    float latitude = 0.0f;
    float longitude = 0.0f;
    struct timespec time;
    float fracional_sec = 0.0f;
    float altitude = 0.0f;
    size_t last_update_ticks = 0;
    bool has_pps = false;
};

#endif // POINT_HPP_INCLUDED
