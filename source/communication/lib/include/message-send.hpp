#ifndef MESSAGESERIALIZER_HPP
#define MESSAGESERIALIZER_HPP

#include "buffer.hpp"
#include "message-header.hpp"

#include <string>
#include <optional>
#include <memory>
#include <queue>

class IHeaderSerializer
{
public:
    virtual PBuffer serialize(const MessageHeader& header) = 0;
    virtual ~IHeaderSerializer() = default;
};

class IMessageSerializer
{
public:
    virtual PBuffer serialize() = 0;
};

class DataSender
{
public:
    DataSender(std::shared_ptr<IHeaderSerializer> header_serializer);

    void push(const char* type, IMessageSerializer& serializer);

    PBuffer next_message();

private:
    std::shared_ptr<IHeaderSerializer> m_header_serializer;
    std::queue<PBuffer> m_messages;
};

#endif // MESSAGESERIALIZER_HPP
