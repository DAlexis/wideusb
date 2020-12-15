#ifndef MESSAGEISERIALIZER_HPP
#define MESSAGEISERIALIZER_HPP

#include "buffer.hpp"
#include "message.hpp"
#include "message-header.hpp"

class IHeaderSerializer
{
public:
    virtual PBuffer serialize(const MessageHeader& header) = 0;
    virtual ~IHeaderSerializer() = default;
};

class IMessageSerializer
{
public:
    virtual PBuffer serialize(const Message* msg) const = 0;
};



#endif // MESSAGEISERIALIZER_HPP
