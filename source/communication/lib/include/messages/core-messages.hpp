#ifndef COREMESSAGES_HPP
#define COREMESSAGES_HPP

#include "communication/message.hpp"

#include <cstdint>

struct StatusRequest : public Message
{
    constexpr static MessageId id = 123;
    StatusRequest() : Message(id) {}

    bool do_blink = false;
};

struct StatusResponse : public Message
{
    constexpr static MessageId id = 456;
    StatusResponse() : Message(id) {}

    bool alive = false;
    uint32_t system_ticks = 0;
    uint32_t free_mem = 0;
};

#endif // COREMESSAGES_HPP
