#include "communication/networking.hpp"
#include <cmath>

#include <cstdio>

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
               uint32_t port,
               OnDataReceivedCallback callback) :
    m_net_service(net_service),
    m_options(my_address),
    m_callback(callback)
{
    m_options.port = port;
    m_net_service.add_socket(*this);
    m_filter.listen_address(m_options.address, 0xFFFFFFFF);
}

Socket::~Socket()
{
    m_net_service.remove_socket(*this);
}

// ISocketUserSide
uint32_t Socket::send(Address destination, PBuffer data)
{
    OutgoingMessage item;
    item.data = data->clone();
    item.receiver = destination;
    item.id = m_net_service.generate_segment_id();
    if (
            m_options.output_queue_limit != 0
            && m_outgoing.size() >= m_options.output_queue_limit
            && m_outgoing.size() >= 2)
    {
        // Remove the oldest one, but not front because it ma be in work
        m_outgoing.erase(std::next(m_outgoing.begin()));
    }
    m_outgoing.push_back(item);

    return item.id;
}

std::optional<ISocketUserSide::IncomingMessage> Socket::get()
{
    if (m_incoming.empty())
        return std::nullopt;

    IncomingMessage data = m_incoming.front();
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

void Socket::drop_currently_sending()
{
    m_state.clear();
}

// ISocketSystemSide
ISocketSystemSide::OutgoingMessage Socket::front()
{
    return m_outgoing.front();
}

bool Socket::has_outgoing_data()
{
    return !m_outgoing.empty();
}

void Socket::pop(bool success)
{
    uint32_t id = m_outgoing.front().id;
    if (m_callback)
        m_callback(id, success);
    m_outgoing.pop_front();
}

void Socket::push(Address sender, PBuffer data)
{
    if (m_options.input_queue_limit != 0 && m_incoming.size() >= m_options.input_queue_limit)
    {
        m_incoming.pop_front();
    }
    IncomingMessage msg;
    msg.sender = sender;
    msg.data = data;
    m_incoming.push_back(msg);
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

SocketState::SocketState()
{
    clear();
}

void SocketState::clear()
{
    state = OutgoingState::clear;
    /*last_send_time = 0;
    repeats_count = 0;
    state_clear = true;*/
}

NetSevice::NetSevice(
        std::shared_ptr<IPhysicalLayer> physical,
        std::shared_ptr<IChannelLayer> channel,
        std::shared_ptr<INetworkLayer> network,
        std::shared_ptr<ITransportLayer> transport,
        std::shared_ptr<IPhysicalLayer> default_transit_physical,
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
    m_sockets.push_back(&socket);
}

void NetSevice::remove_socket(Socket& socket)
{
    for (size_t i = 0; i < m_sockets.size(); i++)
    {
        if (m_sockets[i] == &socket)
        {
            m_sockets[i] = m_sockets.back();
            m_sockets.pop_back();
            break;
        }
    }
}

uint32_t NetSevice::generate_segment_id()
{
    return m_rand_gen();
}

void NetSevice::serve_sockets(uint32_t time_ms)
{
    serve_sockets_input();
    serve_sockets_output(time_ms);
    serve_time_planner(time_ms);
}

void NetSevice::send_ack(Address src, Address dst, uint32_t port, uint32_t ttl, uint32_t ack_id, uint32_t seg_id)
{
    SegmentBuffer sb;
    m_transport->encode(sb, port, seg_id, false, true, ack_id);
    m_network->encode(sb, NetworkOptions(src, dst, ttl));
    m_channel->encode(sb);
    m_physical->send(sb.merge());
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

void NetSevice::serve_sockets_output(uint32_t time_ms)
{
    // Sending cycle
    for (auto& socket : m_sockets)
    {
        SocketState& state = socket->state();
        const SocketOptions& options = socket->get_options();

        if (state.state == SocketState::OutgoingState::clear)
        {
            // Socket is ready to transmit data
            if (!socket->has_outgoing_data())
                continue; // No data

            state.segment_id = m_rand_gen();
            m_time_planner.add(TimePlanner<ISocketSystemSide*>::Task(socket, state.segment_id, time_ms, options.retransmitting_options));

            if (options.need_acknoledgement)
            {
                state.state = SocketState::OutgoingState::repeating_untill_ack;
            } else {
                state.state = SocketState::OutgoingState::repeating_untill_expired_no_ack;
            }
            continue;
        }

        if (!m_time_planner.has_task(state.segment_id))
        {
            // Socket task is expired
            if (state.state == SocketState::OutgoingState::repeating_untill_ack)
            {
                // But socket was waiting for an ack
                socket->pop(false);
                state.clear();
            } else if (state.state == SocketState::OutgoingState::repeating_untill_expired_no_ack)
            {
                // But socket was waiting for timeout or cycles count is over, it does not need an ack
                socket->pop(true);
                state.clear();
            }
        }
    }
}

void NetSevice::serve_sockets_input()
{
    std::vector<DecodedFrame> frames = m_channel->decode(m_physical->incoming());

    for (const auto& frame : frames)
    {
        std::vector<DecodedPacket> packets = m_network->decode(frame.frame);

        for (const auto& packet : packets)
        {
            std::vector<ISocketSystemSide*> receivers = receivers_of_addr(packet.options.receiver);

            if (receivers.empty())
                continue;

            std::vector<DecodedSegment> segments = m_transport->decode(packet.packet);
            for (const auto& segment : segments)
            {
                if (is_already_received(segment.segment_id))
                    continue;

                for (auto receiver : receivers)
                {
                    SocketState& state = receiver->state();
                    const SocketOptions& options = receiver->get_options();

                    if (options.port != segment.port)
                        continue;

                    if (segment.flags & DecodedSegment::Flags::need_ack)
                    {
                        send_ack(options.address, packet.options.sender, options.port, options.ttl, segment.segment_id, segment.segment_id + 1);
                    }

                    // If we got package with acknoledgement and we are waiting for it
                    if (segment.flags & DecodedSegment::Flags::is_ack)
                    {
                        m_time_planner.remove(segment.ack_for_segment_id);
                        // TODO ^last added
                        if (state.state == SocketState::OutgoingState::repeating_untill_ack)
                        {
                            if (segment.ack_for_segment_id == state.segment_id)
                            {
                                // We were waiting for this ack
                                state.clear();
                                receiver->pop(true);
                            }
                        }
                    }

                    if (!segment.segment.empty())
                    {
                        BufferAccessor accessor(segment.segment);
                        receiver->push(packet.options.sender, Buffer::create(accessor));
                    }
                }
            }
        }
    }
}

void NetSevice::serve_time_planner(uint32_t time_ms)
{
    auto batch = m_time_planner.get_batch(time_ms);
    if (batch.tasks.empty())
        return;

    // Classifying packages by its network options
    std::map <NetworkOptions, SegmentBuffer> package_by_addr;
    for (auto socket : batch.tasks)
    {
        ISocketSystemSide::OutgoingMessage front = socket->front();
        SegmentBuffer sb(front.data);

        const NetworkOptions opts(socket->get_options().address, front.receiver, socket->get_options().ttl);
        m_transport->encode(
                    sb,
                    socket->get_options().port,
                    socket->state().segment_id,
                    socket->get_options().need_acknoledgement);
        package_by_addr[opts].push_back(sb);
    }

    // Encoding grouped packages
    for (auto& it: package_by_addr)
    {
        SegmentBuffer& sb = it.second;
        const NetworkOptions& opts = it.first;
        m_network->encode(sb, opts);
        m_channel->encode(sb);
        m_physical->send(sb.merge());
    }
}

std::vector<ISocketSystemSide*> NetSevice::receivers_of_addr(Address addr)
{
    std::vector<ISocketSystemSide*> result;
    for (auto& subscriber : m_sockets)
    {
        if (subscriber->get_address_filter().is_acceptable(addr))
        {
            result.push_back(subscriber);
        }
    }
    return result;
}

