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

NetService::ptr NetService::create(std::unique_ptr<INetServiceRunner> service_runner,
                      IQueueFactory::Ptr queue_factory,
                      std::shared_ptr<INetworkLayer> network,
                      std::shared_ptr<ITransportLayer> transport,
                      std::shared_ptr<IPackageInspector> package_inspector,
                      NetService::RandomGenerator rand_gen)
{
    return ptr(new NetService(std::move(service_runner), queue_factory, network, transport, package_inspector, rand_gen));
}

NetService::NetService(
        std::unique_ptr<INetServiceRunner> service_runner,
        IQueueFactory::Ptr queue_factory,
        std::shared_ptr<INetworkLayer> network,
        std::shared_ptr<ITransportLayer> transport,
        std::shared_ptr<IPackageInspector> package_inspector,
        RandomGenerator rand_gen) :
    m_service_runner(std::move(service_runner)),
    m_queue_factory(queue_factory),
    m_network(network),
    m_transport(transport),
    m_package_inspector(package_inspector),
    m_rand_gen(rand_gen != nullptr ? rand_gen : rand) // std rand() function
{
    if (m_service_runner)
        m_service_runner->set_callback([this]() { serve_sockets(); });
}

NetService::~NetService()
{
    if (m_service_runner)
        m_service_runner->cancel();
}

void NetService::add_interface(std::shared_ptr<NetworkInterface> interface)
{
    m_interfaces.push_back(interface);
    if (m_service_runner)
        interface->physical->set_on_data_callback([this]() { m_service_runner->post_serve_sockets(0ms); });
}

void NetService::add_socket(ISocketSystemSide& socket)
{
    m_sockets.push_back(&socket);
}

void NetService::remove_socket(ISocketSystemSide& socket)
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
    if (m_service_runner)
        m_service_runner->post_serve_sockets(0ms);
}

IQueueFactory::Ptr NetService::get_queue_factory()
{
    return m_queue_factory;
}

std::shared_ptr<INetworkLayer> NetService::get_network_layer()
{
    return m_network;
}

std::shared_ptr<ITransportLayer> NetService::get_transport_layer()
{
    return m_transport;
}

void NetService::serve_sockets()
{
    serve_sockets(std::chrono::steady_clock::now());
    if (m_service_runner)
        m_service_runner->post_serve_sockets(10ms);
}

void NetService::serve_sockets(std::chrono::steady_clock::time_point time_ms)
{
    serve_sockets_input(time_ms);
    serve_sockets_output(time_ms);
}

void NetService::serve_sockets_output(std::chrono::steady_clock::time_point now)
{
    std::chrono::steady_clock::time_point next_output_serve = now + 10s;
    for (auto& socket : m_sockets)
    {
        auto next_time = socket->next_send_time();
        if (!next_time.has_value())
            continue;

        if (*next_time <= now)
        {
            auto seg_buf = socket->pick_outgoing_packet(now);
            if (!seg_buf.has_value())
                continue;

            for (auto& interface : m_interfaces)
            {
                SegmentBuffer copy_for_interface = *seg_buf;
                interface->channel->encode(copy_for_interface);
                auto merged = copy_for_interface.merge();
                if (m_package_inspector)
                {
                    m_package_inspector->inspect_package(merged, "Outgoung data");
                }

                interface->physical->send(merged);
            }
        }

        next_time = socket->next_send_time();
        if (next_time.has_value() && *next_time < next_output_serve)
            next_output_serve = *next_time;
    }
    if (m_service_runner)
        m_service_runner->post_serve_sockets(std::chrono::duration_cast<std::chrono::milliseconds>(next_output_serve - now));
}

void NetService::serve_sockets_input(std::chrono::steady_clock::time_point now)
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

            for (const auto& packet : packets)
            {
                if (m_already_received_packets.check_update(packet.options.id))
                    continue;

                std::vector<ISocketSystemSide*> receivers = receivers_of_addr(packet.options.receiver);

                if (packet.options.ttl > 1 && (receivers.empty() || packet.options.is_broadcast))
                {
                    for (auto& retransmit_interface : m_interfaces)
                    {
                        if (!retransmit_interface->enable_retransmission_here)
                            continue;

                        if (retransmit_interface == interface && !retransmit_interface->enable_retransmission_back)
                            continue;

                        SegmentBuffer retransmitted_data(Buffer::create(packet.packet.size(), packet.packet.data()));
                        NetworkOptions opts = packet.options;
                        opts.ttl--;
                        m_network->encode(retransmitted_data, opts);
                        retransmit_interface->channel->encode(retransmitted_data);
                        retransmit_interface->physical->send(retransmitted_data.merge());
                    }
                }

                if (receivers.empty())
                    continue;

                std::vector<DecodedSegment> segments = m_transport->decode(packet.packet);
                for (const auto& segment : segments)
                {
                    bool duplicate = m_already_received_segments.check_update(segment.segment_id);

                    for (auto receiver : receivers)
                    {
                        const SocketOptions& options = receiver->get_options();

                        if (options.port != segment.port)
                            continue;

                        receiver->receive_segment(now, packet.options.sender, segment, duplicate);
                    }
                }
            }
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

