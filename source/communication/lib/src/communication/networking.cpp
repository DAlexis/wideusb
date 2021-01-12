#include "communication/networking.hpp"
#include <cmath>

void AddressFilter::listen_address(Address addr, Address mask)
{
    Target t;
    t.addr = addr;
    t.mask = mask;
    m_targets.push_back(t);
}

bool AddressFilter::is_acceptable(const Address& addr) const
{
    for (const auto &t : m_targets)
    {
        if ((addr & t.mask) == (t.addr & t.mask))
            return true;
    }
    return false;
}

Socket::Socket(NetSevice& net_service,
               Address my_address,
               Address destination_address,
               uint32_t port,
               OnDataReceivedCallback callback) :
    m_net_service(net_service), m_callback(callback)
{
    m_options.port = port;
    m_options.my_address = my_address;
    m_options.destination_address = destination_address;
    m_net_service.add_socket(*this);
    m_filter.listen_address(m_options.my_address, 0xFFFFFFFF);
}

Socket::~Socket()
{
    m_net_service.remove_socket(*this);
}

// ISocketUserSide
uint32_t Socket::send(PBuffer data)
{
    OutgoingItem item;
    item.data = data->clone();
    item.id = m_id_counter++;
    m_outgoing.push_back(item);
    return item.id;
}

PBuffer Socket::get()
{
    if (m_incoming.empty())
        return nullptr;

    PBuffer data = m_incoming.front();
    m_incoming.pop_front();
    return data;
}

SocketOptions& Socket::options()
{
    return m_options;
}

AddressFilter& Socket::address_filter()
{
    return m_filter;
}

bool Socket::has_data()
{
    return !m_incoming.empty();
}

// ISocketSystemSide
PBuffer Socket::front()
{
    if (m_outgoing.empty())
        return nullptr;
    return m_outgoing.front().data;
}

void Socket::pop(bool success)
{
    uint32_t id = m_outgoing.front().id;
    if (m_callback)
        m_callback(id, success);
    m_outgoing.pop_front();
}

void Socket::push(PBuffer data)
{
    m_incoming.push_back(data);
}

const SocketOptions& Socket::get_options()
{
    return m_options;
}

const AddressFilter& Socket::get_address_filter()
{
    return m_filter;
}

NetSevice::Subscriber::Subscriber()
{
    clear();
}

void NetSevice::Subscriber::clear()
{
    segment_id = 0;
    state = State::clear;
    last_send_time = 0;
    repeats_count = 0;
    state_clear = true;
}

NetSevice::NetSevice(
        std::shared_ptr<IPhysicalLayer> physical,
        std::shared_ptr<IChannelLayer> channel,
        std::shared_ptr<INetworkLayer> network,
        std::shared_ptr<ITransportLayer> transport,
        RandomGenerator rand_gen) :
    m_physical(physical), m_channel(channel), m_network(network), m_transport(transport)
{
    if (rand_gen != nullptr)
        m_rand_gen = rand_gen;
    else
        m_rand_gen = rand; // std rand() function
}

void NetSevice::add_socket(Socket& socket)
{
    Subscriber s;
    s.socket = &socket;
    m_subscribers.push_back(s);
}

void NetSevice::remove_socket(Socket& socket)
{
    for (size_t i = 0; i < m_subscribers.size(); i++)
    {
        if (m_subscribers[i].socket == &socket)
        {
            m_subscribers[i] = m_subscribers.back();
            m_subscribers.pop_back();
            break;
        }
    }
}

void NetSevice::serve_sockets(uint32_t time_ms)
{
    receive_all_sockets();
    send_all_sockets(time_ms);
}

void NetSevice::send_data(PBuffer data, Address src, Address dst, uint32_t port, uint32_t ttl, bool need_ack, uint32_t seg_id)
{
    SegmentBuffer sb(data);
    m_transport->encode(sb, port, seg_id, need_ack);
    m_network->encode(sb, src, dst, ttl);
    m_channel->encode(sb);
    m_physical->send(sb.merge());
}

uint32_t NetSevice::send_ack(Address src, Address dst, uint32_t port, uint32_t ttl, uint32_t ack_id, uint32_t seg_id)
{
    SegmentBuffer sb;
    m_transport->encode(sb, port, seg_id, false, true, ack_id);
    m_network->encode(sb, src, dst, ttl);
    m_channel->encode(sb);
    m_physical->send(sb.merge());
    return seg_id;
}

bool NetSevice::is_already_received(uint32_t segment_id)
{
    bool result = false;
    for (auto id : m_already_received)
    {
        if (id == segment_id)
        {
            result = true;
        }
    }

    if (!result)
    {
        // New segment id, add it to list
        if (m_already_received.size() == m_already_received_capacity)
        {
            m_already_received.pop_back();
        }
        m_already_received.push_front(segment_id);
    }
    return result;
}

void NetSevice::send_all_sockets(uint32_t time_ms)
{
    // Sending cycle
    for (auto& subscriber : m_subscribers)
    {
        ISocketSystemSide* socket = subscriber.socket;
        const SocketOptions& options = socket->get_options();

        // case A: socket sent data and has already been waiting for ack
        if (subscriber.state == Subscriber::State::some_tries_sent)
        {
            if (options.need_acknoledgement)
            {
                // If sending attempts limit is out
                if (time_ms - subscriber.last_send_time > options.timeout_ms)
                {
                    // Delivery failed
                    socket->pop(false);
                    subscriber.clear();
                    continue;
                }
                if (subscriber.repeats_count > options.repeats_limit || time_ms - subscriber.last_send_time < options.repeat_interval_ms)
                {
                    // No need to repeat
                    continue;
                }
            } else {
                if (subscriber.repeats_count > options.repeats_limit)
                {
                    // No-ack package was sent enough times
                    socket->pop(true);
                    subscriber.clear();
                    continue;
                }
                if (time_ms - subscriber.last_send_time < options.repeat_interval_ms)
                {
                    // No need to repeat
                    continue;
                }
            }
        } else {
            // We had not tried to send actual data from this socket
            if (!socket->front())
                continue; // No data

            subscriber.state = Subscriber::State::some_tries_sent;
            subscriber.segment_id = m_rand_gen();
        }

        subscriber.last_send_time = time_ms;
        subscriber.repeats_count++;
        send_data(socket->front(), options.my_address, options.destination_address, options.port, options.ttl, options.need_acknoledgement, subscriber.segment_id);
    }
}

void NetSevice::receive_all_sockets()
{
    std::vector<DecodedFrame> frames = m_channel->decode(m_physical->incoming());

    for (const auto& frame : frames)
    {
        std::vector<DecodedPacket> packets = m_network->decode(frame.frame);

        for (const auto& packet : packets)
        {
            std::vector<Subscriber*> receivers = receivers_of_addr(packet.receiver);

            if (receivers.empty())
                continue;

            std::vector<DecodedSegment> segments = m_transport->decode(packet.packet);
            for (const auto& segment : segments)
            {
                if (is_already_received(segment.segment_id))
                    continue;

                for (auto receiver : receivers)
                {
                    ISocketSystemSide* socket = receiver->socket;
                    const SocketOptions& options = socket->get_options();

                    if (options.port != segment.port)
                        continue;

                    if (segment.flags & DecodedSegment::Flags::need_ack)
                    {
                        send_ack(options.my_address, packet.sender, options.port, options.ttl, segment.segment_id, segment.segment_id + 1);
                    }

                    // If we get package with acknoledgement and we are waiting for it
                    if ((segment.flags & DecodedSegment::Flags::is_ack) && (receiver->state == Subscriber::State::some_tries_sent))
                    {
                        if (segment.ack_for_segment_id == receiver->segment_id)
                        {
                            // We were waiting for this ack
                            receiver->clear();
                            receiver->socket->pop(true);
                        }
                    }

                    if (!segment.segment.empty())
                    {
                        BufferAccessor accessor(segment.segment);
                        socket->push(Buffer::create(accessor));
                    }
                }
            }
        }
    }
}

std::vector<NetSevice::Subscriber*> NetSevice::receivers_of_addr(Address addr)
{
    std::vector<Subscriber*> result;
    for (auto& subscriber : m_subscribers)
    {
        if (subscriber.socket->get_address_filter().is_acceptable(addr))
        {
            result.push_back(&subscriber);
        }
    }
    return result;
}

