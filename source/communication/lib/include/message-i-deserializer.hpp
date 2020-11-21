#ifndef MESSAGEIDESERIALIZER_HPP
#define MESSAGEIDESERIALIZER_HPP

#include "buffer.hpp"
#include "message-header.hpp"

#include <functional>

class IHeaderDeserializer
{
public:
    virtual std::optional<MessageHeader> deserialize(RingBuffer& buffer) = 0;
    virtual ~IHeaderDeserializer() = default;
};

class IMassageDeserializer
{
public:
    virtual bool deserialize(const PBuffer buffer) = 0;
    virtual ~IMassageDeserializer() = default;
};

template<typename T>
class MessageDeserializer : public IMassageDeserializer
{
public:
    using Callback = std::function<void(const T&)>;
    MessageDeserializer(Callback callback) :
        m_callback(callback)
    { }

    bool deserialize(const PBuffer buffer) override
    {
        if (parse_impl(m_message, buffer))
        {
            m_callback(m_message);
            return true;
        } else {
            return false;
        }
    }

    const T& get_message() { return m_message; }

protected:
    virtual bool parse_impl(T& target, const PBuffer buffer) = 0;

    Callback m_callback;
    T m_message;
};

#endif // MESSAGEIDESERIALIZER_HPP
