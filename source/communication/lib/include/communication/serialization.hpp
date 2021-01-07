#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include "communication/message.hpp"

#include "buffer.hpp"

// Concrete message serialization
class MessageSerializer : public SingleMessageService
{
public:
    MessageSerializer(MessageId id) : SingleMessageService(id) { }
    virtual bool serialize(SerialWriteAccessor& accessor, const Message& msg) = 0;
    virtual std::shared_ptr<Message> deserialize(SerialReadAccessor& buffer) = 0;
    virtual ~MessageSerializer() = default;
};


// Final point for receiver message
class MessageReceiver : public SingleMessageService
{
public:
    MessageReceiver(MessageId id) : SingleMessageService(id) { }
    virtual void receive(const Message& msg) = 0;
    virtual ~MessageReceiver() = default;
};

template <typename MessageType>
class MessageCallback : public MessageReceiver
{
public:
    using Callback = std::function<void(const MessageType& msg)>;
    MessageCallback(Callback callback) : m_callback(callback) { }

    void receive(const Message& generic_msg) override
    {
        const MessageType* message = message_cast<MessageType>(&generic_msg);
        if (message == nullptr)
            return;
        m_callback(message);
    }

private:
    Callback m_callback;
};

template <typename MessageType>
class Property : public MessageReceiver
{
public:
    Property() : MessageReceiver(MessageType::id) {}
    void receive(const Message& generic_msg) override
    {
        const MessageType* message = message_cast<MessageType>(&generic_msg);
        if (message == nullptr)
            return;
        m_message = *message;
    }

    MessageType& operator*()
    {
        return m_message;
    }

    MessageType* operator->()
    {
        return &m_message;
    }

private:
    MessageType m_message;
};


template <typename MessageType>
class MessageSetValue : public MessageReceiver
{
};

#endif // SERIALIZATION_HPP
