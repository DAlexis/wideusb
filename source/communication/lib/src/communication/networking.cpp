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
    m_net_service(net_service),
    m_options(NetworkOptions(my_address, destination_address)),
    m_callback(callback)
{
    m_options.port = port;
    m_net_service.add_socket(*this);
    m_filter.listen_address(m_options.network_options.sender, 0xFFFFFFFF);
}

Socket::~Socket()
{
    m_net_service.remove_socket(*this);
}

// ISocketUserSide
uint32_t Socket::send(PBuffer data)
{
    SocketData item;
    item.data = data->clone();
    item.id = m_net_service.generate_segment_id();
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

void Socket::drop_currently_sending()
{
    m_state.clear();
}

// ISocketSystemSide
PBuffer Socket::front()
{
    return m_outgoing.front().data;
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
    m_subscribers.push_back(&socket);
}

void NetSevice::remove_socket(Socket& socket)
{
    for (size_t i = 0; i < m_subscribers.size(); i++)
    {
        if (m_subscribers[i] == &socket)
        {
            m_subscribers[i] = m_subscribers.back();
            m_subscribers.pop_back();
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
    receive_all_sockets();
    send_all_sockets(time_ms);
    serve_time_planner(time_ms);
}

void NetSevice::send_data(PBuffer data, Address src, Address dst, uint32_t port, uint32_t ttl, bool need_ack, uint32_t seg_id)
{
    SegmentBuffer sb(data);
    m_transport->encode(sb, port, seg_id, need_ack);
    m_network->encode(sb, NetworkOptions(src, dst, ttl));
    m_channel->encode(sb);
    m_physical->send(sb.merge());
}

uint32_t NetSevice::send_ack(Address src, Address dst, uint32_t port, uint32_t ttl, uint32_t ack_id, uint32_t seg_id)
{
    SegmentBuffer sb;
    m_transport->encode(sb, port, seg_id, false, true, ack_id);
    m_network->encode(sb, NetworkOptions(src, dst, ttl));
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
        SocketState& state = subscriber->state();
        const SocketOptions& options = subscriber->get_options();

        if (state.state == SocketState::OutgoingState::repeating_untill_ack)
        {
            // Socket is waiting for acknoledgement. Lets check if message expired
            if (!m_time_planner.has_task(state.segment_id))
            {
                subscriber->pop(false);
                state.clear();
            }
        }

        if (state.state == SocketState::OutgoingState::clear)
        {
            // Socket is ready to transmit data
            if (!subscriber->has_outgoing_data())
                continue; // No data

            state.segment_id = m_rand_gen();
            m_time_planner.add(TimePlanner<ISocketSystemSide*>::Task(subscriber, state.segment_id, time_ms, options.retransmitting_options));

            if (options.need_acknoledgement)
            {
                state.state = SocketState::OutgoingState::repeating_untill_ack;
            } else {
                subscriber->pop(true);
            }
        }

/*
        // case A: socket sent data and has already been waiting for ack
        if (state.state == SocketState::OutgoingState::repeating)
        {
            if (options.need_acknoledgement)
            {
                // If sending attempts limit is out
                if (time_ms - state.last_send_time > options.timeout_ms)
                {
                    // Delivery failed
                    subscriber->pop(false);
                    state.clear();
                    continue;
                }
                if (state.repeats_count > options.repeats_limit || time_ms - state.last_send_time < options.repeat_interval_ms)
                {
                    // No need to repeat
                    continue;
                }
            } else {
                if (state.repeats_count > options.repeats_limit)
                {
                    // No-ack package was sent enough times
                    subscriber->pop(true);
                    state.clear();
                    continue;
                }
                if (time_ms - state.last_send_time < options.repeat_interval_ms)
                {
                    // No need to repeat
                    continue;
                }
            }
        } else {
            // We had not tried to send actual data from this socket
            if (!subscriber->has_outgoing_data())
                continue; // No data

            state.state = SocketState::OutgoingState::repeating;
            state.segment_id = m_rand_gen();
        }

        state.last_send_time = time_ms;
        state.repeats_count++;
        send_data(subscriber->front(), options.network_options.sender, options.network_options.receiver, options.port, options.network_options.ttl, options.need_acknoledgement, state.segment_id);
        */
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
                        send_ack(options.network_options.sender, packet.options.sender, options.port, options.network_options.ttl, segment.segment_id, segment.segment_id + 1);
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
                        receiver->push(Buffer::create(accessor));
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
        SegmentBuffer sb(socket->front());
        const NetworkOptions& opts = socket->get_options().network_options;
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
    for (auto& subscriber : m_subscribers)
    {
        if (subscriber->get_address_filter().is_acceptable(addr))
        {
            result.push_back(subscriber);
        }
    }
    return result;
}

