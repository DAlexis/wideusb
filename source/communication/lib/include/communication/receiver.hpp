#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "communication/channel-layer.hpp"
#include "communication/identification.hpp"
#include "communication/serialization.hpp"

#include <map>
#include <memory>
#include <functional>

class IAckSender
{
public:
    virtual void send_ack(MessageKey key) = 0;
    virtual ~IAckSender() = default;
};

// Receive buffer with message
class MessageListener
{
public:
    using AckCallback = std::function<void(MessageKey)>;
    MessageListener(std::shared_ptr<IDestreamificator> destreamificator, std::shared_ptr<IMessageIdentificator> identificator, std::shared_ptr<IAckSender> ack_sender = nullptr);

    void add(std::shared_ptr<MessageSerializer> msg_serializer);
    void add(std::shared_ptr<MessageReceiver> msg_receiver);

    bool receive_one(SerialReadAccessor& read_accessor);

    void subscribe(MessageKey key, AckCallback callback);

private:
    std::shared_ptr<IDestreamificator> m_destreamificator;
    std::shared_ptr<IMessageIdentificator> m_identificator;
    std::shared_ptr<IAckSender> m_ack_sender;

    std::map<MessageId, std::shared_ptr<MessageSerializer>> m_serializers;
    std::map<MessageId, std::shared_ptr<MessageReceiver>> m_receivers;

    std::map<MessageKey, AckCallback> m_subscribers;
};


#endif // RECEIVER_HPP
