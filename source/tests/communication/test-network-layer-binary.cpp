#include "communication/binary/network.hpp"

#include "gtest/gtest.h"

TEST(NetworkLayerBinaryClass, Operating)
{
    const char test_data_1[] = ">Whatever you want here<";
    SegmentBuffer sg1(Buffer::create(sizeof(test_data_1), test_data_1));

    const char test_data_2[] = ">Anything else<";
    SegmentBuffer sg2(Buffer::create(sizeof(test_data_2), test_data_2));

    NetworkLayerBinary net_layer;
    net_layer.encode(sg1, NetworkOptions(0xAAAAAAAA, 0xBBBBBBBB, 4));

    net_layer.encode(sg2, NetworkOptions(0xCCCCCCCC, 0xDDDDDDDD, 3));

    sg1.push_back(sg2);
    std::vector<DecodedPacket> packets = net_layer.decode(BufferAccessor(sg1.merge()));
    ASSERT_EQ(packets.size(), 2);
    ASSERT_EQ(packets[0].options.sender, 0xAAAAAAAA);
    ASSERT_EQ(packets[0].options.receiver, 0xBBBBBBBB);
    ASSERT_EQ(packets[0].options.ttl, 4);
    ASSERT_EQ(packets[0].packet.size(), sizeof(test_data_1));
    PBuffer data1 = Buffer::create(packets[0].packet);
    ASSERT_EQ(0, memcmp(data1->data(), test_data_1, sizeof(test_data_1)));

    ASSERT_EQ(packets[1].options.sender, 0xCCCCCCCC);
    ASSERT_EQ(packets[1].options.receiver, 0xDDDDDDDD);
    ASSERT_EQ(packets[1].options.ttl, 3);
    ASSERT_EQ(packets[1].packet.size(), sizeof(test_data_2));
    PBuffer data2 = Buffer::create(packets[1].packet);
    ASSERT_EQ(0, memcmp(data2->data(), test_data_2, sizeof(test_data_2)));
}
