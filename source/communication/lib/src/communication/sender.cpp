#include "communication/sender.hpp"

MessageTransmitter::MessageTransmitter(std::shared_ptr<IStreamificator> streamificator, std::shared_ptr<IMessageIdentificator> identificator) :
    m_streamificator(streamificator), m_identificator(identificator)
{
}

void MessageTransmitter::add_serializer(std::shared_ptr<MessageSerializer> serializer)
{
    m_serializers[serializer->id()] = serializer;
}

bool MessageTransmitter::put_message(SerialWriteAccessor& accessor, const Message& msg)
{
    auto it = m_serializers.find(msg.id());
    if (it == m_serializers.end())
        return false;

    PBuffer buf = Buffer::create();
    MessageHeader msg_header(msg.id(), 0);
    m_identificator->put_message_id(*buf, msg_header);
    it->second->serialize(*buf, msg);

    return m_streamificator->pack(accessor, buf);
}

bool MessageTransmitter::put_ack(SerialWriteAccessor& accessor, MessageKey key)
{
    PBuffer buf = Buffer::create();
    MessageHeader msg_header(MSG_ID_ACK, key);
    m_identificator->put_message_id(*buf, msg_header);
    return m_streamificator->pack(accessor, buf);
}
/*
void MessageSenderNew::push(const Message* message)
{
}

PBuffer MessageSenderNew::next_message()
{
}

bool MessageSenderNew::empty()
{
}*/
