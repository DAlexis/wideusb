#ifndef HEADERPARSERJSON_HPP
#define HEADERPARSERJSON_HPP

#include "message-receive.hpp"

class HeaderParserJSON : public IHeaderDeserializer
{
public:
    std::optional<MessageHeader> deserialize(RingBuffer& buffer) override;
};

#endif // HEADERPARSERJSON_HPP
