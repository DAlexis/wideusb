#ifndef SENDER_HPP
#define SENDER_HPP

#include "communication/serialization.hpp"
#include "communication/channel-layer.hpp"
#include "communication/identification.hpp"

#include "buffer.hpp"

#include <queue>

class MessageTransmitter
{
public:
    MessageTransmitter(std::shared_ptr<IStreamificator> streamificator, std::shared_ptr<IMessageIdentificator> identificator);

    void add_serializer(std::shared_ptr<MessageSerializer> serializer);

    bool put_message(SerialWriteAccessor& accessor, const Message& msg);

    bool put_ack(SerialWriteAccessor& accessor, MessageKey key);

    /*void push(const Message& message);

    PBuffer next_message();
    bool empty();*/

private:
    std::shared_ptr<IStreamificator> m_streamificator;
    std::shared_ptr<IMessageIdentificator> m_identificator;
    std::map<MessageId, std::shared_ptr<MessageSerializer>> m_serializers;
    std::queue<PBuffer> m_messages;
};

#endif // SENDER_HPP
