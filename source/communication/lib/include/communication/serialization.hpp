#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include "communication/message.hpp"

#include "buffer.hpp"

// Concrete message serialization
class IMessageSerializerNew : public SingleMessageService
{
public:
    virtual PBuffer serialize(const NewMessage* msg) = 0;
    virtual std::shared_ptr<NewMessage> deserialize(PBuffer buffer) = 0;
    virtual ~IMessageSerializerNew() = default;
};


// Final point for receiver message
class IMessageReceiverNew : public SingleMessageService
{
public:
    virtual void receive(const NewMessage*) = 0;
    virtual ~IMessageReceiverNew() = default;
};

template <typename MessageType>
class MessageCallback : public IMessageReceiverNew
{
public:
    using Callback = std::function<void(const MessageType& msg)>;
    MessageCallback(Callback callback) : m_callback(callback) { }

    void receive(const NewMessage* generic_msg) override
    {
        const MessageType* message = message_cast<MessageType>(generic_msg);
        if (message != nullptr)
        {
            m_callback(message);
        }
    }

private:
    Callback m_callback;
};

template <typename MessageType>
class MessageSetValue : public IMessageReceiverNew
{
};

#endif // SERIALIZATION_HPP
