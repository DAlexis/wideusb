#ifndef MESSAGESERIALIZER_HPP
#define MESSAGESERIALIZER_HPP

#include "message-i-serializer.hpp"

#include <string>
#include <optional>
#include <memory>
#include <queue>

class DataSender
{
public:
    DataSender(std::shared_ptr<IHeaderSerializer> header_serializer);

    void push(const char* type, const IMessageSerializer& serializer);

    PBuffer next_message();
    bool empty();

private:
    std::shared_ptr<IHeaderSerializer> m_header_serializer;
    std::queue<PBuffer> m_messages;
};

#endif // MESSAGESERIALIZER_HPP
