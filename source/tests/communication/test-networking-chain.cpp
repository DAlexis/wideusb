#include "wideusb/communication/networking.hpp"
#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"
#include "wideusb-pc/socket-queue-mutex.hpp"

#include "gtest/gtest.h"

class NetworkingTest : public ::testing::Test
{
protected:
    void SetUp() override {
        physical = std::make_shared<PhysicalLayerBuffer>(500);
        interface = std::make_shared<NetworkInterface>(physical, std::make_shared<ChannelLayerBinary>(), true);

        service = NetService::create(nullptr,
                                     std::make_shared<MutexQueueFactory>(),
                                     std::make_shared<NetworkLayerBinary>(),
                                     std::make_shared<TransportLayerBinary>());
        service->add_interface(interface);

    }

    std::shared_ptr<PhysicalLayerBuffer> physical;
    std::shared_ptr<NetService> service;
    std::shared_ptr<NetworkInterface> interface;

    const char test_data[23] = ">Some data here again<";
};

struct NetworkNode
{
    NetworkNode()
    {
        service->add_interface(interface1);
        service->add_interface(interface2);
    }

    std::shared_ptr<PhysicalLayerBuffer> physical1 = std::make_shared<PhysicalLayerBuffer>(500);
    std::shared_ptr<PhysicalLayerBuffer> physical2 = std::make_shared<PhysicalLayerBuffer>(500);
    std::shared_ptr<NetService> service = NetService::create(nullptr,
                                        std::make_shared<MutexQueueFactory>(),
                                        std::make_shared<NetworkLayerBinary>(),
                                        std::make_shared<TransportLayerBinary>());

    std::shared_ptr<NetworkInterface> interface1 = std::make_shared<NetworkInterface>(physical1, std::make_shared<ChannelLayerBinary>(), true);
    std::shared_ptr<NetworkInterface> interface2 = std::make_shared<NetworkInterface>(physical2, std::make_shared<ChannelLayerBinary>(), true);
};

TEST(NetworkingChain, TransmitOverChain)
{
    NetworkNode node1, node2, node3;

    auto tp = std::chrono::steady_clock::now();

    Socket sock1(node1.service, 0x123, 10);
    sock1.options().need_acknoledgement = true;
    Socket sock2(node3.service, 0x321, 10);
    sock2.options().need_acknoledgement = true;

    // Sending data over first socket
    sock1.send(0x321, Buffer::create(1, "X"), tp);
    sock2.send(0x123, Buffer::create(1, "Y"), tp);

    node1.service->serve_sockets(tp);
    node2.service->serve_sockets(tp);
    node3.service->serve_sockets(tp);

    exchange_data({node1.physical2.get(), node2.physical1.get()});
    exchange_data({node2.physical2.get(), node3.physical1.get()});

    tp += 1ms;
    node1.service->serve_sockets(tp);
    node2.service->serve_sockets(tp);
    node3.service->serve_sockets(tp);

    exchange_data({node1.physical2.get(), node2.physical1.get()});
    exchange_data({node2.physical2.get(), node3.physical1.get()});

    tp += 1ms;
    node1.service->serve_sockets(tp);
    node2.service->serve_sockets(tp);
    node3.service->serve_sockets(tp);

    auto sock1_incoming = sock1.get_incoming();
    auto sock2_incoming = sock2.get_incoming();

    ASSERT_TRUE(sock1_incoming.has_value());
    ASSERT_EQ(sock1_incoming->sender, 0x321);

    ASSERT_TRUE(sock2_incoming.has_value());
    ASSERT_EQ(sock2_incoming->sender, 0x123);
}
