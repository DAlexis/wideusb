#include "message-send.hpp"

DataSender::DataSender(std::shared_ptr<IHeaderSerializer> header_serializer) :
    m_header_serializer(header_serializer)
{
}

void DataSender::push(const char* type, const IMessageSerializer& serializer, const Message* msg)
{
    PBuffer body = serializer.serialize(msg);
    MessageHeader header;
    header.message_type = type;
    header.body_size_bytes = body->size();

    PBuffer full_message = m_header_serializer->serialize(header);
    *full_message << *body;
    m_messages.push(full_message);
}

PBuffer DataSender::next_message()
{
    PBuffer result = m_messages.front();
    m_messages.pop();
    return result;
}

bool DataSender::empty()
{
    return m_messages.empty();
}
