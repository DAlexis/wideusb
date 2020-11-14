#ifndef HOSTLISTENER_HPP
#define HOSTLISTENER_HPP

#include "ring-buffer.h"

#include <string>
#include <optional>
#include <memory>

struct MessageHeader
{
    std::string message_type;
    uint32_t body_size_bytes = 0;
};

class IHeaderParser
{
public:
    virtual std::optional<MessageHeader> parse(RingBuffer& buffer) = 0;
    virtual ~IHeaderParser() = default;
};


class HostListener
{
public:
    HostListener(std::shared_ptr<IHeaderParser> header_parser);

    void try_receive(RingBuffer& ring_buffer);

public:
    std::shared_ptr<IHeaderParser> m_header_parser;

};

class IMassageParser
{
public:

};



#endif // HOSTLISTENER_HPP
