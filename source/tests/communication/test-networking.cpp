#include "communication/networking.hpp"
#include "communication/binary/channel.hpp"
#include "communication/binary/network.hpp"
#include "communication/binary/transport.hpp"

#include "gtest/gtest.h"

class NetworkingTest : public ::testing::Test
{
protected:
    void SetUp() override {
        physical = std::make_shared<PhysicalLayerBuffer>(500);
        service = std::make_shared<NetSevice>(physical,
                                              std::make_shared<ChannelLayerBinary>(),
                                              std::make_shared<NetworkLayerBinary>(),
                                              std::make_shared<TransportLayerBinary>());

    }

    std::shared_ptr<PhysicalLayerBuffer> physical;
    std::shared_ptr<NetSevice> service;

    const char test_data[23] = ">Some data here again<";
};

TEST_F(NetworkingTest, HeaderSize)
{
    Socket sock1(*service, 0x12345678, 10);
    sock1.send(0x87654321, Buffer::create(1, "X"));
    service->serve_sockets(0);
    PBuffer data = physical->out_next();
    ASSERT_NE(nullptr, data);
    std::cout << "[     INFO ] " << "Total header size for ack-based message = " << data->size() - 1 << std::endl;

    service->serve_sockets(60001);

    sock1.options().need_acknoledgement = false;
    sock1.drop_currently_sending();
    sock1.send(0x87654321, Buffer::create(1, "X"));
    service->serve_sockets(60001);
    data = physical->out_next();
    ASSERT_NE(nullptr, data);
    std::cout << "[     INFO ] " << "Total header size for no-ack message = " << data->size() - 1 << std::endl;
}

TEST_F(NetworkingTest, BasicOperatingWithAck)
{
    bool data_delivered = false;

    Socket sock1(*service, 0x12345678, 10, nullptr, [&data_delivered](uint32_t, bool success) { data_delivered = success; });
    Socket sock2(*service, 0x87654321, 10);

    Socket sock3(*service, 0x87654321, 11);
    Socket sock4(*service, 0x87654320, 10);

    sock1.send(0x87654321, Buffer::create(sizeof(test_data), test_data));

    // Send data to physical device
    service->serve_sockets(0);

    // Loop back device
    loop_back(*physical);

    // Receive data, send ack
    service->serve_sockets(0);

    ASSERT_FALSE(sock1.has_data());
    ASSERT_TRUE(sock2.has_data());
    ASSERT_FALSE(sock3.has_data());
    ASSERT_FALSE(sock4.has_data());

    auto sock_incoming = sock2.get();

    ASSERT_TRUE(sock_incoming.has_value());
    ASSERT_EQ(sock_incoming->sender, 0x12345678);

    PBuffer data = sock_incoming->data;
    ASSERT_EQ(0, memcmp(data->data(), test_data, sizeof(test_data)));

    // Loop back for acknoledgement
    loop_back(*physical);

    // Receive ack
    service->serve_sockets(0);

    ASSERT_TRUE(data_delivered);
}

TEST_F(NetworkingTest, BasicOperatingNoAck)
{
    bool data_delivered = false;

    Socket sock1(*service, 0x12345678, 10, nullptr, [&data_delivered](uint32_t, bool success) { data_delivered = success; });
    Socket sock2(*service, 0x87654321, 10);

    sock1.options().need_acknoledgement = false;
    sock1.options().retransmitting_options.cycles_count = 1;
    sock1.options().retransmitting_options.timeout = 0;

    sock1.send(0x87654321, Buffer::create(sizeof(test_data), test_data));

    // Send data to physical device
    service->serve_sockets(0);

    // Call the callback: message is delivered
    service->serve_sockets(0);

    // Call the callback: message is delivered
    service->serve_sockets(0);

    ASSERT_TRUE(data_delivered);

    // Loop back device
    loop_back(*physical);

    // Receive data, send ack
    service->serve_sockets(0);

    ASSERT_FALSE(sock1.has_data());
    ASSERT_TRUE(sock2.has_data());

    auto sock_incoming = sock2.get();

    ASSERT_TRUE(sock_incoming.has_value());
    ASSERT_EQ(sock_incoming->sender, 0x12345678);

    PBuffer data = sock_incoming->data;
    ASSERT_EQ(0, memcmp(data->data(), test_data, sizeof(test_data)));
}

TEST_F(NetworkingTest, DataCorruption)
{
    bool data_delivered_1_to_2 = false;
    bool data_delivered_2_to_1 = false;

    TimePlanningOptions timings(1000, 100, 10);

    Socket sock1(*service, 0x12345678, 10, nullptr, [&data_delivered_1_to_2](uint32_t, bool success) { data_delivered_1_to_2 = success; });
    Socket sock2(*service, 0x87654321, 10, nullptr, [&data_delivered_2_to_1](uint32_t, bool success) { data_delivered_2_to_1 = success; });

    sock1.options().retransmitting_options = timings;
    sock2.options().retransmitting_options = timings;

    sock1.send(0x87654321, Buffer::create(sizeof(test_data), test_data));
    service->serve_sockets(0);

    ASSERT_EQ(1, physical->out_queue_size());
    //size_t total_data_size = physical->out_next()->size();

    // Corrupting
    PBuffer data = physical->out_next();
    (*data)[data->size() / 2] += 1;

    physical->in_next(data->data(), data->size());

    // Getting corrupted data
    service->serve_sockets(0);
    ASSERT_FALSE(data_delivered_1_to_2);
    ASSERT_FALSE(data_delivered_2_to_1);
    ASSERT_EQ(0, physical->out_queue_size());

    service->serve_sockets(1001); // Data should be sent again
    ASSERT_EQ(1, physical->out_queue_size());

    // Corrupting
    data = physical->out_next();
    (*data)[data->size() / 3] += 1;

    physical->in_next(data->data(), data->size());
    // Getting corrupted data
    service->serve_sockets(1001);
    ASSERT_FALSE(data_delivered_1_to_2);
    ASSERT_FALSE(data_delivered_2_to_1);

    //loop_back(*physical);
    service->serve_sockets(2101); // Data should be sent again
    ASSERT_EQ(1, physical->out_queue_size());
    loop_back(*physical); // Now sending correct data on physical

    service->serve_sockets(2101); // Data received, sending ack

    ASSERT_TRUE(sock2.has_data());

    auto sock_incoming = sock2.get();

    ASSERT_TRUE(sock_incoming.has_value());
    ASSERT_EQ(sock_incoming->sender, 0x12345678);

    data = sock_incoming->data;

    ASSERT_EQ(0, memcmp(data->data(), test_data, sizeof(test_data)));

    loop_back(*physical); // Now ack over physical layer

    service->serve_sockets(2101); // Now ack should be received

    ASSERT_TRUE(data_delivered_1_to_2);
}
