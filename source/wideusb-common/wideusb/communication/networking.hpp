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
    using ptr = std::shared_ptr<NetService>;
    using RandomGenerator = std::function<uint32_t(void)>;

    static ptr create(std::unique_ptr<INetServiceRunner> service_runner,
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

    std::shared_ptr<INetworkLayer> get_network_layer();
    std::shared_ptr<ITransportLayer> get_transport_layer();

private:
    NetService(
            std::unique_ptr<INetServiceRunner> service_runner,
            IQueueFactory::Ptr queue_factory,
            std::shared_ptr<INetworkLayer> network,
            std::shared_ptr<ITransportLayer> transport,
            std::shared_ptr<IPackageInspector> package_inspector = nullptr,
            RandomGenerator rand_gen = nullptr);

    void serve_sockets_output(std::chrono::steady_clock::time_point time_ms);
    void serve_sockets_input(std::chrono::steady_clock::time_point now);

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

    std::vector<std::shared_ptr<NetworkInterface>> m_interfaces;
};

#endif // NETWORKING_HPP
