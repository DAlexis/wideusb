#include "host-listener.hpp"

HostListener::HostListener(std::shared_ptr<IHeaderParser> header_parser) :
    m_header_parser(header_parser)
{
}

void HostListener::try_receive(RingBuffer& ring_buffer)
{
    auto parsed_header = m_header_parser->parse(ring_buffer);
    if (parsed_header.has_value())
    {

    }
}
