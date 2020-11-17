#ifndef MESSAGEHEADER_HPP
#define MESSAGEHEADER_HPP

#include <string>
#include <optional>

struct MessageHeader
{
    std::string message_type;
    uint32_t body_size_bytes = 0;
};

#endif // MESSAGEHEADER_HPP
