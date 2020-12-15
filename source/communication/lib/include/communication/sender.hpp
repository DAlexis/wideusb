#ifndef SENDER_HPP
#define SENDER_HPP

#include "communication/serialization.hpp"
#include "communication/streamification.hpp"

#include <queue>

class MessageSenderNew
{
public:
    MessageSenderNew(std::shared_ptr<IStreamificator> streamificator);

    void add(std::shared_ptr<IMessageSerializerNew> serializer);

    void push(const NewMessage* message);

    PBuffer next_message();
    bool empty();

private:
    std::shared_ptr<IStreamificator> m_streamificator;
    std::map<MessageId, std::shared_ptr<IMessageSerializerNew>> m_serializers;
    std::queue<PBuffer> m_messages;
};

#endif // SENDER_HPP
