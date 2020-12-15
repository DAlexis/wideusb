#ifndef HEADERPARSERJSON_HPP
#define HEADERPARSERJSON_HPP

#include "message-header.hpp"
#include "message-i-serializer.hpp"
#include "message-i-deserializer.hpp"

class HeaderDeserializerJSON : public IHeaderDeserializer
{
public:
    std::optional<MessageHeader> deserialize(RingBuffer& buffer) override;
};

class HeaderSerializerJSON : public IHeaderSerializer
{
public:
    PBuffer serialize(const MessageHeader& header) override;
};

#endif // HEADERPARSERJSON_HPP
