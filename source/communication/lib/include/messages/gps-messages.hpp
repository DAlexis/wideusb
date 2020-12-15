#ifndef GPSMESSAGES_HPP
#define GPSMESSAGES_HPP

#include "message.hpp"

#include <cstdint>

struct GPSPoint : public Message
{
    bool has_position = false;
    float latitude = 0.0f;
    float longitude = 0.0f;
    float altitude = 0.0f;

    uint64_t sec_epoch = 0.0;
    uint16_t year = 1900;
    uint16_t month = 0;
    uint16_t day = 0;
    uint16_t hour = 0;
    uint16_t min = 0;
    float sec = 0;
};

#endif // GPSMESSAGES_HPP
