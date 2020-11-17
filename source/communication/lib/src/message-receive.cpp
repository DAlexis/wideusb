#include "message-receive.hpp"

MessageReceiver::MessageReceiver(std::shared_ptr<IHeaderDeserializer> header_parser) :
    m_header_parser(header_parser)
{
    reset_state();
}

void MessageReceiver::reset_state()
{
    m_state = State::waiting_header;
    m_next_message_parser = nullptr;
    m_next_body_size = 0;
}

void MessageReceiver::try_receive(RingBuffer& ring_buffer)
{
    switch (m_state)
    {
    case State::waiting_header:
    {
        auto parsed_header = m_header_parser->deserialize(ring_buffer);
        if (parsed_header.has_value())
        {
            auto it = m_message_parsers.find(parsed_header->message_type);
            if (it != m_message_parsers.end())
            {
                m_next_body_size = parsed_header->body_size_bytes;
                m_next_message_parser = it->second.get();
                m_state = State::waiting_body;
            }
        }
    }
    case State::waiting_body:
    {
        if (ring_buffer_data_size(&ring_buffer) >= m_next_body_size)
        {
            m_next_message_parser->deserialize(ring_buffer);
            m_state = State::waiting_header;
        }
    }
    }

}

void MessageReceiver::add_message_parser(const std::string& name, std::shared_ptr<IMassageDeserializer> message_parser)
{

}
