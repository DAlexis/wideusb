#ifndef NETWORKING_HPP
#define NETWORKING_HPP

#include "wideusb/communication/socket.hpp"
#include "wideusb/communication/net-srv-runner.hpp"
#include "wideusb/communication/network-types.hpp"
#include "wideusb/communication/network-interface.hpp"
#include "wideusb/communication/i-physical-layer.hpp"
#include "wideusb/communication/i-channel-layer.hpp"
#include "wideusb/communication/i-network-layer.hpp"
#include "wideusb/communication/i-transport-layer.hpp"
#include "wideusb/communication/i-package-inspector.hpp"
#include "wideusb/communication/utils/time-group-maker.hpp"
#include "wideusb/utils/caching-set.hpp"
#include "wideusb/buffer.hpp"

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <optional>
#include <chrono>

class NetService
{
public:
    using RandomGenerator = std::function<uint32_t(void)>;

    NetService(
            std::unique_ptr<INetServiceRunner> service_runner,
            IQueueFactory::Ptr queue_factory,
            std::shared_ptr<INetworkLayer> network,
            std::shared_ptr<ITransportLayer> transport,
            std::shared_ptr<IPackageInspector> package_inspector = nullptr,
            RandomGenerator rand_gen = nullptr);
    ~NetService();

    void add_interface(std::shared_ptr<NetworkInterface> interface);

    void serve_sockets();
    void serve_sockets(std::chrono::steady_clock::time_point time_ms);

    void add_socket(ISocketSystemSide& socket);
    void remove_socket(ISocketSystemSide& socket);

    SegmentID generate_segment_id();

    void on_socket_send();

    IQueueFactory::Ptr get_queue_factory();

private:

    void serve_sockets_output(std::chrono::steady_clock::time_point time_ms);
    void serve_sockets_input();
    void serve_time_planner(std::chrono::steady_clock::time_point time_ms);

    void send_ack(std::shared_ptr<NetworkInterface> interface, Address src, Address dst, Port port, uint32_t ttl, uint32_t ack_id, SegmentID seg_id);

    std::vector<ISocketSystemSide*> receivers_of_addr(Address addr);

    std::unique_ptr<INetServiceRunner> m_service_runner;
    IQueueFactory::Ptr m_queue_factory;

    std::shared_ptr<INetworkLayer> m_network;
    std::shared_ptr<ITransportLayer> m_transport;
    std::shared_ptr<IPackageInspector> m_package_inspector;

    std::vector<ISocketSystemSide*> m_sockets;
    RandomGenerator m_rand_gen;

    CachingSet<SegmentID> m_already_received_segments{100};
    CachingSet<PacketID> m_already_received_packets{100};

    struct SocketSendTask
    {
        SocketSendTask(const ISocketSystemSide::OutgoingMessage& message, ISocketSystemSide* socket) :
            message(message), socket(socket)
        { }

        ISocketSystemSide::OutgoingMessage message;
        ISocketSystemSide* socket;
    };

    TimePlanner<SocketSendTask> m_time_planner; // TODO

    std::vector<std::shared_ptr<NetworkInterface>> m_interfaces;
};

#endif // NETWORKING_HPP
