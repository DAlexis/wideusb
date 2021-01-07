#include "communication/receiver.hpp"

MessageListener::MessageListener(std::shared_ptr<IDestreamificator> destreamificator, std::shared_ptr<IMessageIdentificator> identificator, std::shared_ptr<IAckSender> ack_sender) :
    m_destreamificator(destreamificator), m_identificator(identificator), m_ack_sender(ack_sender)
{
}

void MessageListener::add(std::shared_ptr<MessageSerializer> msg_serializer)
{
    m_serializers[msg_serializer->id()] = msg_serializer;
}

void MessageListener::add(std::shared_ptr<MessageReceiver> msg_receiver)
{
    m_receivers[msg_receiver->id()] = msg_receiver;
}

bool MessageListener::receive_one(SerialReadAccessor& read_accessor)
{
    std::optional<PBuffer> result = m_destreamificator->unpack(read_accessor);

    if (!result.has_value())
        return false;

    BufferAccessor buff_acc(*result);

    // Add get address here

    std::optional<MessageHeader> header = m_identificator->get_message_id(buff_acc);
    if (!header.has_value())
        return false;

    // If received acknoledgement
    if (header->id == MSG_ID_ACK)
    {
        auto it_subscriber = m_subscribers.find(header->key);
        if (it_subscriber != m_subscribers.end())
        {
            it_subscriber->second(header->key);
            m_subscribers.erase(it_subscriber);
        }
        return true;
    }

    auto it_serial = m_serializers.find(header->id);
    auto it_receiver = m_receivers.find(header->id);

    if (it_serial == m_serializers.end() || it_receiver == m_receivers.end())
        return false;

    std::shared_ptr<Message> msg = it_serial->second->deserialize(buff_acc);
    if (msg == nullptr)
        return false;

    it_receiver->second->receive(*msg);
    return true;
}

void MessageListener::subscribe(MessageKey key, AckCallback callback)
{
    m_subscribers[key] = callback;
}
