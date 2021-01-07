#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <functional>
#include <map>

using MessageId = int;

class Message
{
public:
    Message(MessageId id) : m_id(id) {}

    MessageId id() const { return m_id; }

protected:
    Message& operator=(const Message&) = default;

private:
    MessageId m_id;
};


struct SomeMessage : Message
{
    constexpr static int id = 123;
    SomeMessage() : Message(id) {}

    int data = 123;
};

template<typename MessageType>
const MessageType* message_cast(const Message* msg)
{
    if (std::remove_pointer<MessageType>::type::id != msg->id())
        return nullptr;
    else
        return static_cast<const MessageType*>(msg);
}

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
