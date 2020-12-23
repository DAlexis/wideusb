#ifndef COREMESSAGES_HPP
#define COREMESSAGES_HPP

//#include "message.hpp"

#include <cstdint>

struct StatusRequest //: public Message
{
    bool do_blink = false;
};

struct StatusResponse //: public Message
{
    bool alive = false;
    uint32_t system_ticks = 0;
    uint32_t free_mem = 0;
};

#endif // COREMESSAGES_HPP
