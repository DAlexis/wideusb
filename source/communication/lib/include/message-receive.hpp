#ifndef HOSTLISTENER_HPP
#define HOSTLISTENER_HPP

#include "buffer.hpp"
#include "message-header.hpp"
#include "message-i-deserializer.hpp"

#include <string>
#include <map>
#include <optional>
#include <functional>
#include <memory>

struct RingBuffer;

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
