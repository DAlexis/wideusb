#include "wideusb/communication/socket.hpp"
#include "wideusb/communication/networking.hpp"

Socket::Socket(NetService& net_service,
           Address my_address,
           Port port,
           OnIncomingDataCallback incoming_cb,
           OnDataReceivedCallback received_cb) :
    m_net_service(net_service),
    m_options(my_address),
    m_incoming_cb(incoming_cb),
    m_received_cb(received_cb)
{
    m_incoming_queue = m_net_service.get_queue_factory()->produce_incomming_queue(m_options.input_queue_limit);
    m_outgoing_queue = m_net_service.get_queue_factory()->produce_outgoing_queue(m_options.output_queue_limit);

    m_options.port = port;
    m_net_service.add_socket(*this);
    m_filter.listen_address(m_options.address, 0xFFFFFFFF);
}

Socket::~Socket()
{
    m_net_service.remove_socket(*this);
}

SocketOptions& Socket::options()
{
    return m_options;
}

AddressFilter& Socket::address_filter()
{
    return m_filter;
}

const SocketOptions& Socket::get_options()
{
    return m_options;
}

const AddressFilter& Socket::get_address_filter()
{
    return m_filter;
}

SocketState& Socket::state()
{
    return m_state;
}

// ISocketUserSide
SegmentID Socket::send(Address destination, PBuffer data)
{
    OutgoingMessage item;
    item.data = data->clone();
    item.receiver = destination;
    item.id = m_net_service.generate_segment_id();
    size_t out_size = m_outgoing_queue->size();
    if (
            m_options.output_queue_limit != 0
            && out_size >= m_options.output_queue_limit)
    {
        m_outgoing_queue->pop();
    }
    m_outgoing_queue->push(item);
    m_net_service.on_socket_send();
    return item.id;
}

std::optional<ISocketUserSide::IncomingMessage> Socket::get_incoming()
{
    if (m_incoming_queue->size() == 0)
        return std::nullopt;

    IncomingMessage data = m_incoming_queue->front();
    m_incoming_queue->pop();
    return data;
}

bool Socket::has_incoming()
{
    return m_incoming_queue->size() != 0;
}

void Socket::drop_currently_sending()
{
    // TODO tell it to net service
    m_state.clear();
}

// ISocketSystemSide
ISocketSystemSide::OutgoingMessage Socket::get_outgoing()
{
    auto result = m_outgoing_queue->front();
    m_outgoing_queue->pop();
    return result;
}

bool Socket::has_outgoing()
{
    return m_outgoing_queue->size() != 0;
}

void Socket::notify_outgoing_sending_done(SegmentID id, bool success)
{
    if (m_received_cb)
        m_received_cb(id, success);
}

void Socket::push_incoming(Address sender, PBuffer data)
{
    if (m_options.input_queue_limit != 0 && m_incoming_queue->size() >= m_options.input_queue_limit)
    {
        m_incoming_queue->pop();
    }
    IncomingMessage msg;
    msg.sender = sender;
    msg.data = data;
    m_incoming_queue->push(msg);
    if (m_incoming_cb)
        m_incoming_cb(*this);
}


SocketState::SocketState()
{
    clear();
}

void SocketState::clear()
{
    state = OutgoingState::clear;
}
