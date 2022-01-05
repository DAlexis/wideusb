#include "wideusb/communication/networking.hpp"
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

Socket::Socket(NetService& net_service,
               Address my_address,
               uint32_t port,
               OnIncomingDataCallback incoming_cb,
               OnDataReceivedCallback received_cb) :
    m_net_service(net_service),
    m_options(my_address),
    m_incoming_cb(incoming_cb),
    m_received_cb(received_cb)
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
SegmentID Socket::send(Address destination, PBuffer data)
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
    m_net_service.on_socket_send();
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
    if (m_received_cb)
        m_received_cb(id, success);
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
    if (m_incoming_cb)
        m_incoming_cb(*this);
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
}

NetService::NetService(
        std::shared_ptr<INetworkLayer> network,
        std::shared_ptr<ITransportLayer> transport,
        OnAnySocketSendCallback on_any_socket_send,
        std::shared_ptr<IPackageInspector> package_inspector,
        RandomGenerator rand_gen) :
    m_network(network),
    m_transport(transport),
    m_package_inspector(package_inspector),
    m_rand_gen(rand_gen != nullptr ? rand_gen : rand), // std rand() function
    m_on_any_socket_send_callback(on_any_socket_send)
{
}

void NetService::add_interface(std::shared_ptr<NetworkInterface> interface)
{
    m_interfaces.push_back(interface);
    interface->physical->on_network_service_connected(*this);
}

void NetService::add_socket(Socket& socket)
{
    m_sockets.push_back(&socket);
}

void NetService::remove_socket(Socket& socket)
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

uint32_t NetService::generate_segment_id()
{
    return m_rand_gen();
}

void NetService::on_socket_send()
{
    if (m_on_any_socket_send_callback)
        m_on_any_socket_send_callback();
}

void NetService::serve_sockets(std::chrono::steady_clock::time_point time_ms)
{
    serve_sockets_input();
    serve_sockets_output(time_ms);
    serve_time_planner(time_ms);
}

void NetService::send_ack(std::shared_ptr<NetworkInterface> interface, Address src, Address dst, Port port, uint32_t ttl, uint32_t ack_id, SegmentID seg_id)
{
    SegmentBuffer sb;
    m_transport->encode(sb, port, seg_id, false, true, ack_id);
    m_network->encode(sb, NetworkOptions(src, dst, m_rand_gen(), ttl));
    interface->channel->encode(sb);
    interface->physical->send(sb.merge());
}

void NetService::serve_sockets_output(std::chrono::steady_clock::time_point time_ms)
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

void NetService::serve_sockets_input()
{
    for (auto& interface : m_interfaces)
    {
        if (m_package_inspector)
        {
            PBuffer incoming = Buffer::create(interface->physical->incoming().size());
            interface->physical->incoming().get(incoming->data(), incoming->size());
            m_package_inspector->inspect_package(incoming, "Incoming data");
        }

        std::vector<DecodedFrame> frames = interface->channel->decode(interface->physical->incoming());


        for (const auto& frame : frames)
        {
            std::vector<DecodedPacket> packets = m_network->decode(frame.frame);

            std::vector<DecodedPacket> transit_packets;

            for (const auto& packet : packets)
            {
                if (m_already_received_packets.check_update(packet.options.id))
                    continue;

                std::vector<ISocketSystemSide*> receivers = receivers_of_addr(packet.options.receiver);

                if (receivers.empty())
                    continue;

                if (receivers.empty() || packet.options.retranslate_if_received)
                {
                    for (auto& retransmit_interface : m_interfaces)
                    {
                        if (!retransmit_interface->retransmission_to_interface)
                            continue;

                        // Packet is transit
                        if (packet.options.ttl == 1)
                            continue;

                        SegmentBuffer retransmitted_data(Buffer::create(packet.packet.size(), packet.packet.data()));
                        NetworkOptions opts = packet.options;
                        opts.ttl--;
                        m_network->encode(retransmitted_data, opts);
                        retransmit_interface->channel->encode(retransmitted_data);
                        retransmit_interface->physical->send(retransmitted_data.merge());
                    }
                    continue;
                }

                std::vector<DecodedSegment> segments = m_transport->decode(packet.packet);
                for (const auto& segment : segments)
                {
                    if (m_already_received_segments.check_update(segment.segment_id))
                        continue;

                    for (auto receiver : receivers)
                    {
                        SocketState& state = receiver->state();
                        const SocketOptions& options = receiver->get_options();

                        if (options.port != segment.port)
                            continue;

                        // TODO change this code to send ack many times
                        if (segment.flags & DecodedSegment::Flags::need_ack)
                        {
                            send_ack(interface, options.address, packet.options.sender, options.port, options.ttl, segment.segment_id, segment.segment_id + 1);
                        }

                        // If we got package with acknoledgement and we are waiting for it
                        if (segment.flags & DecodedSegment::Flags::is_ack)
                        {
                            m_time_planner.remove(segment.ack_for_segment_id);

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
}

void NetService::serve_time_planner(std::chrono::steady_clock::time_point time_ms)
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

        const NetworkOptions opts(socket->get_options().address, front.receiver, 0, socket->get_options().ttl);
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
        NetworkOptions opts = it.first;
        opts.id = m_rand_gen(); // Changing id to random
        m_network->encode(sb, opts);
        for (auto& interface : m_interfaces)
        {
            interface->channel->encode(sb);
            interface->physical->send(sb.merge());
        }
    }
}

std::vector<ISocketSystemSide*> NetService::receivers_of_addr(Address addr)
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

