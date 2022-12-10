#include "wideusb/communication/socket.hpp"
#include "wideusb/communication/networking.hpp"

#include <limits>

Socket::Socket(std::shared_ptr<NetService> net_service,
           Address my_address,
           Port port,
           OnIncomingDataCallback incoming_cb,
           OnDataReceivedCallback received_cb) :
    m_net_service(net_service),
    m_options(my_address),
    m_incoming_cb(incoming_cb),
    m_received_cb(received_cb)
{
    m_incoming_queue = m_net_service->get_queue_factory()->produce_incomming_queue(m_options.input_queue_limit);
    m_outgoing_queue = m_net_service->get_queue_factory()->produce_outgoing_queue(m_options.output_queue_limit);

    m_options.port = port;
    m_net_service->add_socket(*this);
    m_filter.listen_address(m_options.address, 0xFFFFFFFF);
}

Socket::~Socket()
{
    m_net_service->remove_socket(*this);
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

// ISocketUserSide
SegmentID Socket::send(Address destination, PBuffer data, std::chrono::steady_clock::time_point now)
{
    size_t out_size = m_outgoing_queue->size();
    if (
            m_options.output_queue_limit != 0
            && out_size >= m_options.output_queue_limit)
    {
        m_outgoing_queue->pop();
    }

    OutgoingMessage item;
    item.receiver = destination;
    item.id = m_net_service->generate_segment_id();
    item.next_send = now;
    item.cycles_count = m_options.retransmitting_options.cycles_count;

    item.packet = data;

    m_net_service->get_transport_layer()->encode(
        item.packet,
        m_options.port,
        item.id,
        m_options.need_acknoledgement
    );

    m_net_service->get_network_layer()->encode(item.packet,
                                               NetworkOptions(m_options.address, destination, m_net_service->generate_segment_id(), m_options.ttl, m_options.is_broadcast));

    m_outgoing_queue->push(item);
    m_net_service->on_socket_send();
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
    if (m_outgoing_queue->size() > 0)
    {
        m_outgoing_queue->pop();
    }
}

// ISocketSystemSide

std::optional<std::chrono::steady_clock::time_point> Socket::next_send_time()
{
    if (m_outgoing_queue->size() == 0)
        return std::nullopt;

    return m_outgoing_queue->front().next_send;
}

std::optional<SegmentBuffer> Socket::pick_outgoing_packet(std::chrono::steady_clock::time_point now)
{
    // TODO add transport packages cancatenation if many packages in output queue
    if (m_outgoing_queue->size() == 0)
    {
        return std::nullopt;
    }

    auto& front = m_outgoing_queue->front();

    front.sended_times++;

    if (front.sended_times < front.cycles_count)
    {
        // Next try delay applicable only if next try is possible
        float ms_count = float(m_net_service->generate_segment_id()) / float(std::numeric_limits<uint32_t>::max()) * m_options.retransmitting_options.duration.count();
        auto random_delay = std::chrono::milliseconds(int64_t(ms_count));

        front.next_send = now + m_options.retransmitting_options.interval + random_delay;
    }

    if (front.sended_times == front.cycles_count
            && !m_options.need_acknoledgement
            && !front.is_only_ack)
    {
        if (m_received_cb)
            m_received_cb(front.id, true);
    }

    if (front.sended_times > front.cycles_count)
    {
        if (m_options.need_acknoledgement && m_received_cb)
            m_received_cb(front.id, false);

        m_outgoing_queue->pop();
        return pick_outgoing_packet(now);
//        return std::nullopt;
    }

    return front.packet;
}

void Socket::receive_segment(std::chrono::steady_clock::time_point now, Address sender, const DecodedSegment& segment, bool duplicate)
{
    if (!duplicate &&
        // Not empty ack package
        !((segment.flags & DecodedSegment::Flags::is_ack) && segment.segment.size() == 0)
       )
    {
        // First put data to queue
        IncomingMessage incoming_msg;
        incoming_msg.sender = sender;
        BufferAccessor acc_copy(segment.segment);
        incoming_msg.data = Buffer::create(acc_copy);
        m_incoming_queue->push(incoming_msg);

        // Than call the callback. It may put response to outgoing queue
        if (m_incoming_cb)
            m_incoming_cb(*this);
    }

    if (segment.flags & DecodedSegment::Flags::need_ack)
    {
        // send ack
        SegmentBuffer sb;
        if (m_outgoing_queue->size() != 0)
        {
            // TODO repack next outgoing. May be standalone ack qeueue that are waiting some time
        } else {
        }

        OutgoingMessage outgoing;
        outgoing.id = segment.segment_id+1;
        outgoing.cycles_count = 1;
        outgoing.next_send = now;
        outgoing.receiver = sender;
        outgoing.is_only_ack = true;

        // SegmentBuffer& segment, uint32_t port, uint32_t segment_id, bool need_ack = false, bool make_ack = false, uint32_t ack_id = 0
        m_net_service->get_transport_layer()->encode(sb, m_options.port, outgoing.id, false, true, segment.segment_id);
        m_net_service->get_network_layer()->encode(sb, NetworkOptions(m_options.address, sender, m_net_service->generate_segment_id(), m_options.ttl, false));

        outgoing.packet = sb;
        m_outgoing_queue->push_front(outgoing);
    }

    if (segment.flags & DecodedSegment::Flags::is_ack)
    {
        if (segment.ack_for_segment_id == m_outgoing_queue->front().id)
        {
            m_outgoing_queue->pop();
            if (m_received_cb)
                m_received_cb(segment.ack_for_segment_id, true);
        }
    }
}
