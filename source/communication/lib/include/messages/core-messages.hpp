#ifndef COREMESSAGES_HPP
#define COREMESSAGES_HPP

#include <cstdint>

struct StatusRequest
{
    bool do_blink = false;
};

struct StatusResponse
{
    bool alive = false;
    uint32_t system_ticks = 0;
    uint32_t free_mem = 0;
};

#endif // COREMESSAGES_HPP
