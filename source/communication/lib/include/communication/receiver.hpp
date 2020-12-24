#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "communication/serialization.hpp"
#include "communication/streamification.hpp"

#include <map>
#include <memory>

// Receive buffer with message
class ReceiverNew
{
public:
    ReceiverNew(std::shared_ptr<IStreamificator> streamificator);
    void add(std::shared_ptr<IMessageSerializerNew> msg_serializer);
    void add(std::shared_ptr<IMessageReceiverNew> msg_receiver);

    void receive(SerialReadAccessor& msg);
private:
    std::shared_ptr<IStreamificator> m_streamificator;

    std::map<MessageId, std::shared_ptr<IMessageSerializerNew>> m_serializers;
    std::map<MessageId, std::shared_ptr<IMessageReceiverNew>> m_receivers;
};


#endif // RECEIVER_HPP
