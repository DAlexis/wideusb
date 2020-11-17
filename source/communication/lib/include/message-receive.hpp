#ifndef HOSTLISTENER_HPP
#define HOSTLISTENER_HPP

#include "ring-buffer.h"
#include "message-header.hpp"

#include <string>
#include <map>
#include <optional>
#include <functional>
#include <memory>

class IHeaderDeserializer
{
public:
    virtual std::optional<MessageHeader> deserialize(RingBuffer& buffer) = 0;
    virtual ~IHeaderDeserializer() = default;
};

class IMassageDeserializer
{
public:
    virtual bool deserialize(RingBuffer& buffer) = 0;
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

    bool deserialize(RingBuffer& buffer) override
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
    virtual bool parse_impl(T& target, RingBuffer& buffer) = 0;

    Callback m_callback;
    T m_message;
};

class MessageReceiver
{
public:
    MessageReceiver(std::shared_ptr<IHeaderDeserializer> header_parser);

    void reset_state();

    void try_receive(RingBuffer& ring_buffer);

    void add_message_parser(const std::string& name, std::shared_ptr<IMassageDeserializer> message_parser);

private:
    enum class State {
        waiting_header,
        waiting_body
    };
    std::shared_ptr<IHeaderDeserializer> m_header_parser;
    std::map<std::string, std::shared_ptr<IMassageDeserializer>> m_message_parsers;

    State m_state = State::waiting_header;
    uint32_t m_next_body_size = 0;

    IMassageDeserializer* m_next_message_parser = nullptr;
};





#endif // HOSTLISTENER_HPP
