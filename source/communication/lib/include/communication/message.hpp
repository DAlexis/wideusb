#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <functional>
#include <map>

using MessageId = int;

struct NewMessage
{
    NewMessage(MessageId id) : id(id) {}
    const MessageId id;
};


struct SomeMessage : NewMessage
{
    constexpr static int id = 123;
    SomeMessage() : NewMessage(id) {}

    int data = 123;
};

template<typename MessageType>
const MessageType* message_cast(const NewMessage* msg)
{
    if (MessageType::id != msg)
        return nullptr;
    else
        return static_cast<const MessageType*>(msg);
}

/*
// Extract message from possibly inconsistent stream
class IMessageExtractor
{
public:
    virtual std::optional<PBuffer> extract(RingBuffer& ring_buffer) = 0;
};
*/

class ISingleMessageService
{
public:
    virtual MessageId id() = 0;
    virtual ~ISingleMessageService() = default;
};

class SingleMessageService : public ISingleMessageService
{
public:
    SingleMessageService(MessageId id) : m_id(id) {}
    MessageId id() override { return m_id; }
private:
    MessageId m_id;
};



#endif // MESSAGE_HPP
